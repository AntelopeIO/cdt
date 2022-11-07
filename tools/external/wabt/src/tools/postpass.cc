/*
 * Copyright 2016 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "src/apply-names.h"
#include "src/binary-reader.h"
#include "src/binary-writer.h"
#include "src/binary-reader-ir.h"
#include "src/error-handler.h"
#include "src/feature.h"
#include "src/generate-names.h"
#include "src/ir.h"
#include "src/leb128.h"
#include "src/option-parser.h"
#include "src/stream.h"
#include "src/validator.h"
#include "src/wast-lexer.h"
#include "src/wat-writer.h"

using namespace wabt;

static int s_verbose;
static std::string s_infile;
static std::string s_outfile;
static Features s_features;
static WriteBinaryOptions s_write_binary_options;
static std::unique_ptr<FileStream> s_log_stream;

static const char s_description[] =
R"(  Read a file in the WebAssembly binary format, strip bss or any data segment that is only initialized to zeros, and other post processing.

  $ eosio-pp test.wasm -o test.stripped.wasm

  # or original replacement
  $ wasm2wat test.wasm
)";

static void ParseOptions(int argc, char** argv) {
  OptionParser parser("postprocess", s_description);

  parser.AddOption('v', "verbose", "Use multiple times for more info", []() {
    s_verbose++;
    s_log_stream = FileStream::CreateStdout();
  });
  parser.AddHelpOption();
  parser.AddOption(
      'o', "output", "FILENAME",
      "Output file for the generated wast file, by default use stdout",
      [](const char* argument) {
        s_outfile = argument;
        ConvertBackslashToSlash(&s_outfile);
      });
  parser.AddArgument("filename", OptionParser::ArgumentCount::One,
                     [](const char* argument) {
                       s_infile = argument;
                       ConvertBackslashToSlash(&s_infile);
                     });
  parser.Parse(argc, argv);
}

uint32_t GetHeapPtr( Module& mod, const std::vector<uint8_t>& buff ) {
   size_t offset = mod.GetGlobal(Var(1))->init_expr.begin()->loc.offset;
   uint32_t heap_ptr;
   ReadS32Leb128(buff.data()+offset+4, buff.data()+offset+9, &heap_ptr);
   return heap_ptr;
}

uint32_t GetStackPtr( Module& mod, const std::vector<uint8_t>& buff ) {
   size_t offset = mod.GetGlobal(Var(0))->init_expr.begin()->loc.offset;
   uint32_t stack_ptr;
   ReadS32Leb128(buff.data()+offset+4, buff.data()+offset+9, &stack_ptr);
   return stack_ptr;
}

inline bool IsZeroed(const DataSegment* ds) {
   for ( auto d : ds->data ) {
      if (d != 0)
         return false;
   }
   return true;
}

std::vector<DataSegment*> StripZeroedData( std::vector<DataSegment*>&& ds, size_t& fix_bytes ) {
   for ( auto itr=ds.begin(); itr != ds.end();) {
      if (IsZeroed(*itr)) {
         fix_bytes += (*itr)->data.size();
	 itr = ds.erase(itr); 
      } else {
         ++itr;
      }
   }
   return ds;
}

inline std::vector<uint8_t> FillFromSegments(const std::vector<DataSegment*>& segments) {
  std::vector<uint8_t> memory;  
  auto last_segment = segments.back();
  ConstExpr* ce = reinterpret_cast<ConstExpr*>(&(last_segment->offset.front()));
  memory.resize(ce->const_.u32+last_segment->data.size());

  for (auto ds : segments) {
    auto offset = reinterpret_cast<ConstExpr*>(&(ds->offset.front()))->const_.u32;
    for (std::size_t i=0; i < ds->data.size(); ++i) {
      memory[offset+i] = ds->data[i];
    }
  }

  return memory;
}

inline DataSegment* CreateSegment(uint32_t offset, uint8_t* start, std::size_t size) {
   DataSegment* segment = new DataSegment();
   Const c;
   c.I32(0);
   c.u32 = offset;
   std::unique_ptr<Expr> ce(new ConstExpr(c));
   segment->memory_var = Var(0);
   segment->offset = ExprList{std::move(ce)};
   segment->data.resize(size);
   std::memcpy(segment->data.data(), start, size);
   return segment;
}

inline std::vector<DataSegment*> CreateSegments(std::vector<uint8_t> memory) {
   std::vector<DataSegment*> segments;

   std::size_t f=0;
   // look for the first non-zero entry
   for (; f < memory.size(); ++f) {
      if (memory[f] != 0)
         break;
   }

   uint32_t zero_span = 0;
   uint32_t break_size = 1024;
   uint32_t last_offset = f;

   for (std::size_t i=f; i < memory.size(); ++i) {
      zero_span = memory[i] == 0 ? zero_span + 1 : 0;
      std::size_t size = i - last_offset;
      if (zero_span > 8 || (size > 1024)) {
         segments.push_back(CreateSegment(last_offset, &memory[last_offset], size));
         last_offset = i;
         zero_span = 0;
         i += size;
         continue;
      }
   }
   segments.push_back(CreateSegment(last_offset, &memory[last_offset], memory.size()-last_offset));

   return segments;
}

void AddHeapPointerData( Module& mod, size_t fixup, const std::vector<uint8_t>& buff, DataSegment& ds ) {
   uint32_t heap_ptr  = ((GetHeapPtr(mod, buff)) + 7) & ~7; // align to 8 bytes
   Const c;
   c.I32(0);
   std::unique_ptr<Expr> ce(new ConstExpr(c));
   ds.memory_var = Var(0);
   ds.offset = ExprList{std::move(ce)};
   uint8_t* dat = reinterpret_cast<uint8_t*>(&heap_ptr);
   ds.data = std::vector<uint8_t>{dat[0],
                                  dat[1],
                                  dat[2],
                                  dat[3]};
   mod.data_segments.push_back(&ds);
}

void WriteBufferToFile(string_view filename,
                       const OutputBuffer& buffer) {
  buffer.WriteToFile(filename);
}

int ProgramMain(int argc, char** argv) {
  Result result;

  InitStdio();
  ParseOptions(argc, argv);

  std::vector<uint8_t> file_data;
  bool stub = false;
  std::unique_ptr<FileStream> s_log_stream_s;
  result = ReadFile(s_infile.c_str(), &file_data);
  DataSegment _hds;
  if (Succeeded(result)) {
    ErrorHandlerFile error_handler(Location::Type::Binary);
    Module module;
    const bool kStopOnFirstError = true;
    ReadBinaryOptions options(s_features, s_log_stream_s.get(),
                              stub, kStopOnFirstError,
                              stub);
    result = ReadBinaryIr(s_infile.c_str(), file_data.data(),
                          file_data.size(), &options, &error_handler, &module);

    if (Succeeded(result)) {
      size_t fixup = 0;
      auto pre_memory =  FillFromSegments(module.data_segments);
      auto segments   = CreateSegments(pre_memory);
      if (pre_memory != FillFromSegments(segments)) {
        std::cerr << "Fractured Memory Failed, not applying optimizations" << std::endl;
        module.data_segments = StripZeroedData(std::move(module.data_segments), fixup);
      } else {
        module.data_segments = StripZeroedData(std::move(segments), fixup);
      }
      AddHeapPointerData(module, fixup, file_data, _hds);
     if (Succeeded(result)) {
      MemoryStream stream(s_log_stream.get());
      result =
          WriteBinaryModule(&stream, &module, &s_write_binary_options);

      if (Succeeded(result)) {
        if (s_outfile.empty()) {
          s_outfile = s_infile;
        }
        WriteBufferToFile(s_outfile.c_str(), stream.output_buffer());
      }
    }
   }

  }
  return result != Result::Ok;
}

int main(int argc, char** argv) {
  WABT_TRY
  return ProgramMain(argc, argv);
  WABT_CATCH_BAD_ALLOC_AND_EXIT
}
