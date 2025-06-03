#pragma once
#include <eosio/gen.hpp>
#include <eosio/ppcallbacks.hpp>

#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/QualTypeNames.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"

#include <eosio/utils.hpp>
#include <eosio/whereami/whereami.hpp>
#include <eosio/abi.hpp>

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <set>
#include <map>
#include <array>
#include <jsoncons/json.hpp>

using namespace llvm;
using namespace eosio;
using namespace eosio::cdt;
using jsoncons::json;
using jsoncons::ojson;

namespace eosio { namespace cdt {
   class abigen : public generation_utils {
      std::set<std::string> checked_actions;
      public:
      using generation_utils::generation_utils;

      static abigen& get() {
         static abigen ag;
         return ag;
      }

      void set_abi_version(int major, int minor) {
         _abi.version_major = major;
         _abi.version_minor = minor;
      }

      void add_typedef( const clang::QualType& t ) {
         abi_typedef ret;
         ret.new_type_name = get_base_type_name( t );
         auto td = get_type_alias(t);
         if (td.empty())
            return;
         ret.type = translate_type(td[0]);
         if(!is_builtin_type(td[0]))
            add_type(td[0]);
         _abi.typedefs.insert(ret);
      }

      void add_action( const clang::CXXRecordDecl* decl ) {
         abi_action ret;
         auto action_name = decl->getEosioActionAttr()->getName();

         if (!checked_actions.insert(get_action_name(decl)).second)
            CDT_CHECK_WARN(!rcs[get_action_name(decl)].empty(), "abigen_warning", decl->getLocation(), "Action <"+get_action_name(decl)+"> does not have a ricardian contract");

         if (!suppress_ricardian_warnings)
            if (rcs[get_action_name(decl)].empty())
               // TODO:
               std::cout << "Warning, action <"+get_action_name(decl)+"> does not have a ricardian contract\n";

         ret.ricardian_contract = rcs[get_action_name(decl)];

         if (action_name.empty()) {
            validate_name(decl->getName().str(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); });
            ret.name = decl->getName().str();
         }
         else {
            validate_name( action_name.str(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); });
            ret.name = action_name.str();
         }
         ret.type = decl->getName().str();
         _abi.actions.insert(ret);
      }

      void add_action( const clang::CXXMethodDecl* decl ) {
         abi_action ret;

         auto action_name = decl->getEosioActionAttr()->getName();

         if (!checked_actions.insert(get_action_name(decl)).second)
            CDT_CHECK_WARN(!rcs[get_action_name(decl)].empty(), "abigen_warning", decl->getLocation(), "Action <"+get_action_name(decl)+"> does not have a ricardian contract");

         if (!suppress_ricardian_warnings)
            if (rcs[get_action_name(decl)].empty())
               // TODO
               std::cout << "Warning, action <"+get_action_name(decl)+"> does not have a ricardian contract\n";

         ret.ricardian_contract = rcs[get_action_name(decl)];

         if (action_name.empty()) {
            validate_name( decl->getNameAsString(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); } );
            ret.name = decl->getNameAsString();
         }
         else {
            validate_name( action_name.str(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); } );
            ret.name = action_name.str();
         }
         ret.type = decl->getNameAsString();
         _abi.actions.insert(ret);
         // TODO
         if (translate_type(decl->getReturnType()) != "void") {
            /** TODO after LLVM 9 update uncomment this code and use new error handling for pretty clang style errors
            if (decl->getReturnType() == decl->getDeclaredReturnType())
            */
            add_type(decl->getReturnType());
            _abi.action_results.insert({get_action_name(decl), translate_type(decl->getReturnType())});
            /*
            else {
               std::cout << "Error, currently in eosio.cdt v2.0 `auto` is not allowed for actions\n";
               throw abigen_ex;
            }
            */
         }
      }

      void add_call( const clang::CXXRecordDecl* decl ) {
         abi_call ret;
         auto call_name = decl->getEosioCallAttr()->getName();

         if (call_name.empty()) {
            validate_name(decl->getName().str(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); });
            ret.name = decl->getName().str();
         }
         else {
            validate_name( call_name.str(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); });
            ret.name = call_name.str();
         }
         ret.type = decl->getName().str();
         _abi.calls.insert(ret);
      }

      void add_call( const clang::CXXMethodDecl* decl ) {
         abi_call ret;

         auto call_name = decl->getEosioCallAttr()->getName();

         if (call_name.empty()) {
            validate_name( decl->getNameAsString(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); } );
            ret.name = decl->getNameAsString();
         }
         else {
            validate_name( call_name.str(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); } );
            ret.name = call_name.str();
         }
         ret.type = decl->getNameAsString();
         _abi.calls.insert(ret);
         if (translate_type(decl->getReturnType()) != "void") {
            add_type(decl->getReturnType());
            _abi.call_results.insert({get_call_name(decl), translate_type(decl->getReturnType())});
         }
      }

      void add_tuple(const clang::QualType& type) {
         auto pt = llvm::dyn_cast<clang::ElaboratedType>(type.getTypePtr());
         auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>((pt) ? pt->desugar().getTypePtr() : type.getTypePtr());
         if (!tst) {
            CDT_INTERNAL_ERROR("template specialization failure");
         }
         abi_struct tup;
         tup.name = get_type(type);
         for (int i = 0; i < tst->getNumArgs(); ++i) {
            clang::QualType ftype = std::get<clang::QualType>(get_template_argument(type, i));
            add_type(ftype);
            tup.fields.push_back( {"field_"+std::to_string(i),
                  translate_type(ftype)} );
         }
         _abi.structs.insert(tup);
      }

      void add_pair(const clang::QualType& type) {
         for (int i = 0; i < 2; ++i) {
            clang::QualType ftype = std::get<clang::QualType>(get_template_argument(type, i));
            std::string ty = translate_type(ftype);
            add_type(ftype);
         }
         abi_struct pair;
         pair.name = get_type(type);
         pair.fields.push_back( {"first", get_template_argument_as_string(type)} );
         pair.fields.push_back( {"second", get_template_argument_as_string(type, 1)} );
         add_type(std::get<clang::QualType>(get_template_argument(type)));
         add_type(std::get<clang::QualType>(get_template_argument(type, 1)));
         _abi.structs.insert(pair);
      }

      void add_map(const clang::QualType& type) {
         for (int i = 0; i < 2; ++i) {
            add_type(std::get<clang::QualType>(get_template_argument(type, i)));
         }
         abi_struct map_info;
         std::string name = get_type(type);
         map_info.name = name.substr(0, name.length() - 2);
         auto remove_ending_brackets = [&]( std::string name ) {
            int i = name.length()-1;
            for (; i >= 0; i--)
               if ( name[i] != '[' && name[i] != ']' )
                  break;
            return name.substr(0,i+1);
         };
         map_info.name = remove_ending_brackets(name);
         map_info.fields.push_back( {"first", get_template_argument_as_string(type)} );
         map_info.fields.push_back( {"second", get_template_argument_as_string(type, 1)} );
         add_type(std::get<clang::QualType>(get_template_argument(type)));
         add_type(std::get<clang::QualType>(get_template_argument(type, 1)));
         _abi.structs.insert(map_info);
      }

      void add_struct( const clang::CXXRecordDecl* decl, const std::string& rname="" ) {
         abi_struct ret;
         if ( decl->getNumBases() == 1 ) {
            ret.base = get_type(decl->bases_begin()->getType());
            add_type(decl->bases_begin()->getType());
         }
         for ( auto field : decl->fields() ) {
            if ( field->getName() == "transaction_extensions") {
               abi_struct ext;
               ext.name = "extension";
               ext.fields.push_back( {"type", "uint16"} );
               ext.fields.push_back( {"data", "bytes"} );
               ret.fields.push_back( {"transaction_extensions", "extension[]"});
               _abi.structs.insert(ext);
            }
            else {
               ret.fields.push_back({field->getName().str(), get_type(field->getType())});
               add_type(field->getType());
            }
         }
         if (!rname.empty())
            ret.name = rname;
         else
            ret.name = decl->getName().str();

         const auto res = _abi.structs.insert(ret);
      }

      void add_struct( const clang::CXXMethodDecl* decl ) {
         abi_struct new_struct;
         new_struct.name = decl->getNameAsString();

         if (decl->isEosioCall()) {
            // Add call_data_header definition to structs set
            abi_struct data_header;
            data_header.name = "call_data_header";
            if (_abi.structs.count(data_header) == 0) {
               data_header.fields.push_back({"version", "uint32"});
               data_header.fields.push_back({"func_name", "uint64"});
               _abi.structs.insert(data_header);
            }

            // Add header field as the first field to the method struct
            new_struct.fields.push_back({"header", "call_data_header"});
         }

         for (auto param : decl->parameters() ) {
            auto param_type = param->getType().getNonReferenceType().getUnqualifiedType();
            new_struct.fields.push_back({param->getNameAsString(), get_type(param_type)});
            add_type(param_type);
         }
         _abi.structs.insert(new_struct);
      }

      std::string to_index_type( std::string t ) {
         return "i64";
      }

      void add_table( const clang::CXXRecordDecl* decl ) {
         tables.insert(decl);
         abi_table t;
         t.type = decl->getNameAsString();
         auto table_name = decl->getEosioTableAttr()->getName();
         if (!table_name.empty()) {
            validate_name( table_name.str(), [&](auto s) { CDT_ERROR("abigen_error", decl->getLocation(), s); } );
            t.name = table_name.str();
         }
         else {
            t.name = t.type;
         }
         ctables.insert(t);
      }

      void add_table( uint64_t name, const clang::CXXRecordDecl* decl ) {
         abi_table t;
         t.type = decl->getNameAsString();
         t.name = name_to_string(name);
         _abi.tables.insert(t);
      }

      void add_clauses( const std::vector<std::pair<std::string, std::string>>& clauses ) {
         for ( auto clp : clauses ) {
            _abi.ricardian_clauses.push_back({std::get<0>(clp), std::get<1>(clp)});
         }
      }

      void add_contracts( const std::map<std::string, std::string>& rc ) {
         rcs = rc;
      }

      void add_variant( const clang::QualType& t ) {
         abi_variant var;
         auto pt = llvm::dyn_cast<clang::ElaboratedType>(t.getTypePtr());
         auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>((pt) ? pt->desugar().getTypePtr() : t.getTypePtr());
         var.name = get_type(t);
         for (int i=0; i < tst->getNumArgs(); ++i) {
            var.types.push_back(get_template_argument_as_string( t, i ));
            add_type(std::get<clang::QualType>(get_template_argument( t, i )));
         }
         _abi.variants.insert(var);
      }

      inline void adding_explicit_nested_dispatcher(const clang::QualType& inside_type, int depth, std::string & inside_type_name){
         if(is_explicit_nested(inside_type)){  // inside type is still explict nested  <<>>
            inside_type_name = add_explicit_nested_type(inside_type, depth + 1);
         } else if(is_explicit_container(inside_type)) {  // inside type is single container,  only one <>
            inside_type_name = add_explicit_nested_type(inside_type, depth + 1);
         }else if (is_builtin_type(translate_type(inside_type))){   // inside type is builtin
            inside_type_name = translate_type(inside_type);
         } else if (is_aliasing(inside_type)) { // inside type is an alias
            add_typedef(inside_type);
            inside_type_name = get_base_type_name( inside_type );
         }   else if (is_template_specialization(inside_type, {})) {
            add_struct(inside_type.getTypePtr()->getAsCXXRecordDecl(), get_template_name(inside_type));
            inside_type_name = get_template_name(inside_type);
         }else if (inside_type.getTypePtr()->isRecordType()) {
            add_struct(inside_type.getTypePtr()->getAsCXXRecordDecl());
            inside_type_name = inside_type.getTypePtr()->getAsCXXRecordDecl()->getNameAsString();
         } else {
            std::string errstring = "adding_explicit_nested_dispatcher: this inside type  ";
            errstring += inside_type.getAsString();
            errstring += " is unexpected, maybe not supported so far. \n";
            CDT_INTERNAL_ERROR(errstring);
         }
      }

      void add_explicit_nested_linear(const clang::QualType& type, int depth, abi_typedef & abidef, std::string & ret, const std::string & tname, bool & gottype){
         ret += tname + "_";
         auto inside_type = std::get<clang::QualType>(get_template_argument(type));
         std::string inside_type_name;
         adding_explicit_nested_dispatcher(inside_type, depth, inside_type_name);
         if(inside_type_name != ""){
            ret += inside_type_name;
            abidef.type = inside_type_name + ( (tname == "optional") ? "?" : "[]" );
            gottype = true;
         }
      }

      void add_explicit_nested_map(const clang::QualType& type, int depth, abi_typedef & abidef, std::string & ret, const std::string & tname, bool & gottype){
         ret += tname + "_";
         clang::QualType inside_type[2];
         std::string inside_type_name[2];
         for(int i = 0; i < 2; ++i){
            inside_type[i] = std::get<clang::QualType>(get_template_argument(type, i));
            adding_explicit_nested_dispatcher(inside_type[i], depth, inside_type_name[i]);
         }

         if(inside_type_name[0] != "" && inside_type_name[1] != ""){
            ret += inside_type_name[0] + "_" + inside_type_name[1];
            abidef.type = "pair_" + inside_type_name[0] + "_" + inside_type_name[1] + "[]";

            abi_struct kv;
            kv.name = "pair_" + inside_type_name[0] + "_" + inside_type_name[1];
            kv.fields.push_back( {"first", inside_type_name[0]} );
            kv.fields.push_back( {"second", inside_type_name[1]} );
            _abi.structs.insert(kv);

            gottype = true;
         }
      }

      void add_explicit_nested_pair(const clang::QualType& type, int depth, abi_typedef & abidef, std::string & ret, const std::string & tname, bool & gottype){
         ret += tname + "_";
         clang::QualType inside_type[2];
         std::string inside_type_name[2];
         for(int i = 0; i < 2; ++i){
            inside_type[i] = std::get<clang::QualType>(get_template_argument(type, i));
            adding_explicit_nested_dispatcher(inside_type[i], depth, inside_type_name[i]);
         }

         if(inside_type_name[0] != "" && inside_type_name[1] != ""){
            ret += inside_type_name[0] + "_" + inside_type_name[1];
            abidef.type = "pair_" + inside_type_name[0] + "_" + inside_type_name[1];

            abi_struct pair;
            pair.name = "pair_" + inside_type_name[0] + "_" + inside_type_name[1];
            pair.fields.push_back( {"first", inside_type_name[0]} );
            pair.fields.push_back( {"second", inside_type_name[1]} );
            _abi.structs.insert(pair);

            gottype = true;
         }
      }

      void add_explicit_nested_tuple(const clang::QualType& type, int argcnt, int depth, abi_typedef & abidef, std::string & ret, const std::string & tname, bool & gottype){
         ret += tname + "_";
         std::vector<clang::QualType> inside_type(argcnt);
         std::vector<std::string> inside_type_name(argcnt);
         for(int i = 0; i < argcnt; ++i){
            inside_type[i] = std::get<clang::QualType>(get_template_argument(type, i));
            adding_explicit_nested_dispatcher(inside_type[i], depth, inside_type_name[i]);
         }
         bool allgot = true;
         for(auto & inside_tn : inside_type_name) {
            if(inside_tn == "") allgot = false;
         }
         if(allgot){
            abi_struct tup;
            tup.name = "tuple_";
            abidef.type = "tuple_";
            for (int i = 0; i < argcnt; ++i) {
               ret += inside_type_name[i] + (i < (argcnt - 1) ? "_" : "");
               abidef.type += inside_type_name[i] + (i < (argcnt - 1) ? "_" : "");
               tup.name += inside_type_name[i] + (i < (argcnt - 1) ? "_" : "");
               tup.fields.push_back( {"field_"+std::to_string(i), inside_type_name[i]} );
            }
            _abi.structs.insert(tup);

            gottype = true;
         }
      }

      void add_explicit_nested_array(const clang::QualType& type, int depth, abi_typedef & abidef, std::string & ret, const std::string & tname, bool & gottype){
         ret += tname + "_";
         auto inside_type = std::get<clang::QualType>(get_template_argument(type));
         std::string inside_type_name;
         adding_explicit_nested_dispatcher(inside_type, depth, inside_type_name);

         if(inside_type_name != ""){
            ret += inside_type_name + "_";
            std::string orig = type.getAsString();
            auto pos1 = orig.find_last_of(',');
            auto pos2 = orig.find_last_of('>');
            std::string digits = orig.substr(pos1 + 1, pos2 - pos1 - 1);
            digits.erase(std::remove(digits.begin(), digits.end(), ' '), digits.end());
            ret += digits;
            abidef.type = inside_type_name + "[" + digits + "]" ;
            gottype = true;
         }
      }

      void add_explicit_nested_variant(const clang::QualType& type, int argcnt, int depth, abi_typedef & abidef, std::string & ret, const std::string & tname, bool & gottype){
         ret += tname + "_";
         std::vector<clang::QualType> inside_type(argcnt);
         std::vector<std::string> inside_type_name(argcnt);
         for(int i = 0; i < argcnt; ++i){
            inside_type[i] = std::get<clang::QualType>(get_template_argument(type, i));
            adding_explicit_nested_dispatcher(inside_type[i], depth, inside_type_name[i]);
         }
         bool allgot = true;
         for(auto & inside_tn : inside_type_name) {
            if(inside_tn == "") allgot = false;
         }

         if(allgot){
            abi_variant var;
            var.name = "variant_";
            abidef.type = "variant_";
            for (int i = 0; i < argcnt; ++i) {
               ret += inside_type_name[i] + (i < (argcnt - 1) ? "_" : "");
               abidef.type += inside_type_name[i] + (i < (argcnt - 1) ? "_" : "");
               var.name += inside_type_name[i] + (i < (argcnt - 1) ? "_" : "");
               var.types.push_back( inside_type_name[i]);
            }
            _abi.variants.insert(var);

            gottype = true;
         }
      }

      // return combined typename, and mid-type will be add automatically, only be used on explicit nested type has <<>> or more
      std::string add_explicit_nested_type(const clang::QualType& type, int depth = 0){
         abi_typedef abidef;
         std::string ret = "B_";
         bool gottype = false;
         auto pt = llvm::dyn_cast<clang::ElaboratedType>(type.getTypePtr());
         if(auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>(pt ? pt->desugar().getTypePtr() : type.getTypePtr())){
            if(auto rt = llvm::dyn_cast<clang::RecordType>(tst->desugar())){
               if(auto * decl = rt->getDecl()){
                  std::string tname = decl->getName().str();
                  if(tname == "vector" || tname == "set" || tname == "deque" || tname == "list" || tname == "optional") {
                     add_explicit_nested_linear(type, depth,  abidef, ret, tname, gottype);
                  } else if (tname == "map" ) {
                     add_explicit_nested_map(type, depth, abidef, ret, tname, gottype);
                  } else if (tname == "pair" ) {
                     add_explicit_nested_pair(type, depth, abidef, ret, tname, gottype);
                  } else if (tname == "tuple")  {
                     int argcnt = tst->getNumArgs();
                     add_explicit_nested_tuple(type, argcnt, depth, abidef, ret, tname, gottype);
                  } else if (tname == "array")  {
                     add_explicit_nested_array(type, depth, abidef, ret, tname, gottype);
                  } else if (tname == "variant") {
                     int argcnt = tst->getNumArgs();
                     add_explicit_nested_variant(type, argcnt, depth, abidef, ret, tname, gottype);
                  }
               }
            }
         }

         if(!gottype) {
            std::string errstring = "add_explicit_nested_type failed to fetch type from ";
            errstring += type.getAsString();
            CDT_INTERNAL_ERROR(errstring);
            return "";
         }
         ret +="_E";
         abidef.new_type_name = ret;  // the name is combined from container name and low layer type
         if(depth > 0) _abi.typedefs.insert(abidef);
         return ret;
      }

      void add_type( const clang::QualType& t ) {
         if (evaluated.count(t.getTypePtr()))
            return;
         evaluated.insert(t.getTypePtr());
         auto type = get_ignored_type(t);
         if(is_explicit_nested(t)){
            add_explicit_nested_type(t.getNonReferenceType());
            return;
         }
         if (!is_builtin_type(translate_type(type))) {
            if (is_aliasing(type)) {
               add_typedef(type);
            }
            else if (is_template_specialization(type, {"vector", "set", "deque", "list", "optional", "binary_extension", "ignore"})) {
               add_type(std::get<clang::QualType>(get_template_argument(type)));
            }
            else if (is_template_specialization(type, {"map"}))
               add_map(type);
            else if (is_template_specialization(type, {"pair"}))
               add_pair(type);
            else if (is_template_specialization(type, {"tuple"}))
               add_tuple(type);
            else if (is_template_specialization(type, {"array"}) )
               add_type(std::get<clang::QualType>(get_template_argument(type, 0)));
            else if (is_template_specialization(type, {"variant"}))
               add_variant(type);
            else if (is_template_specialization(type, {})) {
               add_struct(type.getTypePtr()->getAsCXXRecordDecl(), get_template_name(type));
            }
            else if (type.getTypePtr()->isRecordType())
               add_struct(type.getTypePtr()->getAsCXXRecordDecl());
         }
      }

      std::string generate_json_comment() {
         std::stringstream ss;
         ss << "This file was generated with eosio-abigen.";
         ss << " DO NOT EDIT ";
         return ss.str();
      }

      ojson struct_to_json( const abi_struct& s ) {
         ojson o;
         o["name"] = s.name;
         o["base"] = s.base;
         o["fields"] = ojson::array();
         for ( auto field : s.fields ) {
            ojson f;
            f["name"] = field.name;
            f["type"] = field.type;
            o["fields"].push_back(f);
         }
         return o;
      }

      ojson variant_to_json( const abi_variant& v ) {
         ojson o;
         o["name"] = v.name;
         o["types"] = ojson::array();
         for ( auto ty : v.types ) {
            o["types"].push_back( ty );
         }
         return o;
      }

      ojson typedef_to_json( const abi_typedef& t ) {
         ojson o;
         o["new_type_name"] = t.new_type_name;
         o["type"]          = t.type;
         return o;
      }

      ojson action_to_json( const abi_action& a ) {
         ojson o;
         o["name"] = a.name;
         o["type"] = a.type;
         o["ricardian_contract"] = a.ricardian_contract;
         return o;
      }

      ojson call_to_json( const abi_call& c ) {
         ojson o;
         o["name"] = c.name;
         o["type"] = c.type;
         return o;
      }

      ojson clause_to_json( const abi_ricardian_clause_pair& clause ) {
         ojson o;
         o["id"] = clause.id;
         o["body"] = clause.body;
         return o;
      }

      ojson table_to_json( const abi_table& t ) {
         ojson o;
         o["name"] = t.name;
         o["type"] = t.type;
         o["index_type"] = "i64";
         o["key_names"] = ojson::array();
         o["key_types"] = ojson::array();
         return o;
      }

      ojson action_result_to_json( const abi_action_result& result ) {
         ojson o;
         o["name"] = result.name;
         o["result_type"] = result.type;
         return o;
      }

      ojson call_result_to_json( const abi_call_result& result ) {
         ojson o;
         o["name"] = result.name;
         o["result_type"] = result.type;
         return o;
      }

      bool is_empty() {
         std::set<abi_table> set_of_tables;
         for ( auto t : ctables ) {
            bool has_multi_index = false;
            for ( auto u : _abi.tables ) {
               if (t.type == u.type) {
                  has_multi_index = true;
                  break;
               }
               set_of_tables.insert(u);
            }
            if (!has_multi_index)
               set_of_tables.insert(t);
         }
         for ( auto t : _abi.tables ) {
            set_of_tables.insert(t);
         }

         return _abi.structs.empty() && _abi.typedefs.empty() && _abi.actions.empty() && _abi.calls.empty()  && set_of_tables.empty() && _abi.ricardian_clauses.empty() && _abi.variants.empty();
      }

      ojson to_json() {
         ojson o;
         o["____comment"] = generate_json_comment();
         o["version"]     = _abi.version_string();
         o["structs"]     = ojson::array();
         auto remove_suffix = [&]( std::string name ) {
            int i = name.length()-1;
            for (; i >= 0; i--)
               if ( name[i] != '[' && name[i] != ']' && name[i] != '?' && name[i] != '$' )
                  break;
            return name.substr(0,i+1);
         };

         std::set<abi_table> set_of_tables;
         for ( auto t : ctables ) {
            bool has_multi_index = false;
            for ( auto u : _abi.tables ) {
               if (t.type == u.type) {
                  has_multi_index = true;
                  break;
               }
               set_of_tables.insert(u);
            }
            if (!has_multi_index)
               set_of_tables.insert(t);
         }
         for ( auto t : _abi.tables ) {
            set_of_tables.insert(t);
         }

         std::function<std::string(const std::string&)> get_root_name;
         get_root_name = [&] (const std::string& name) {
            for (auto td : _abi.typedefs)
               if (remove_suffix(name) == td.new_type_name)
                  return get_root_name(td.type);
            return name;
         };

         auto validate_struct = [&]( abi_struct as ) {
            if ( is_builtin_type(_translate_type(as.name)) )
               return false;
            if ( is_reserved(_translate_type(as.name)) ) {
               return false;
            }
            for ( auto s : _abi.structs ) {
               for ( auto f : s.fields ) {
                  if (as.name == _translate_type(remove_suffix(f.type)))
                     return true;
               }
               for ( auto v : _abi.variants ) {
                  for ( auto vt : v.types ) {
                     if (as.name == _translate_type(remove_suffix(vt)))
                        return true;
                  }
               }
               if (get_root_name(s.base) == as.name)
                  return true;
            }
            for ( auto a : _abi.actions ) {
               if (as.name == _translate_type(a.type))
                  return true;
            }
            for ( auto a : _abi.calls ) {
               if (as.name == _translate_type(a.type))
                  return true;
            }
            for( auto t : set_of_tables ) {
               if (as.name == _translate_type(t.type))
                  return true;
            }
            for( auto td : _abi.typedefs ) {
               if (as.name == _translate_type(remove_suffix(td.type)))
                  return true;
            }
            for( auto ar : _abi.action_results ) {
               if (as.name == _translate_type(ar.type))
                  return true;
            }
            for( auto ar : _abi.call_results ) {
               if (as.name == _translate_type(ar.type))
                  return true;
            }
            return false;
         };

         auto validate_types = [&]( abi_typedef td ) {
            for ( auto as : _abi.structs )
               if (validate_struct(as)) {
                  for ( auto f : as.fields )
                     if ( remove_suffix(f.type) == td.new_type_name )
                        return true;
                  if (as.base == td.new_type_name)
                     return true;
               }

            for ( auto v : _abi.variants ) {
               for ( auto vt : v.types ) {
                  if ( remove_suffix(vt) == td.new_type_name )
                     return true;
               }
            }
            for ( auto t : _abi.tables )
               if ( t.type == td.new_type_name )
                  return true;
            for ( auto a : _abi.actions )
               if ( a.type == td.new_type_name )
                  return true;
            for ( auto a : _abi.calls )
               if ( a.type == td.new_type_name )
                  return true;
            for ( auto _td : _abi.typedefs )
               if ( remove_suffix(_td.type) == td.new_type_name )
                  return true;
            for ( auto ar : _abi.action_results ) {
               if ( ar.type == td.new_type_name )
                  return true;
            }
            for ( auto ar : _abi.call_results )
               if ( ar.type == td.new_type_name )
                  return true;
            return false;
         };

         for ( auto s : _abi.structs ) {
            const auto res = validate_struct(s);
            if (res)
               o["structs"].push_back(struct_to_json(s));
         }
         o["types"]       = ojson::array();
         for ( auto t : _abi.typedefs ) {
            if (validate_types(t))
               o["types"].push_back(typedef_to_json( t ));
         }
         o["actions"]     = ojson::array();
         for ( auto a : _abi.actions ) {
            o["actions"].push_back(action_to_json( a ));
         }
         if (_abi.version_major > abi_call_version_major ||
             _abi.version_major == abi_call_version_major && _abi.version_minor >= abi_call_version_minor) { // sync call
            o["calls"] = ojson::array();
            for ( auto a : _abi.calls ) {
               o["calls"].push_back(call_to_json( a ));
            }

            o["call_results"] = ojson::array();
            for ( auto ar : _abi.call_results ) {
               o["call_results"].push_back(call_result_to_json( ar ));
            }
         }
         o["tables"]     = ojson::array();
         for ( auto t : set_of_tables ) {
            o["tables"].push_back(table_to_json( t ));
         }
         o["ricardian_clauses"]  = ojson::array();
         for ( auto rc : _abi.ricardian_clauses ) {
            o["ricardian_clauses"].push_back(clause_to_json( rc ));
         }
         o["variants"]   = ojson::array();
         for ( auto v : _abi.variants ) {
            o["variants"].push_back(variant_to_json( v ));
         }
         o["abi_extensions"]     = ojson::array();
         if (_abi.version_major == 1 && _abi.version_minor >= 2) {
            o["action_results"]  = ojson::array();
            for ( auto ar : _abi.action_results ) {
               o["action_results"].push_back(action_result_to_json( ar ));
            }
         }
         return o;
      }

      private:
         abi                                   _abi;
         std::set<const clang::CXXRecordDecl*> tables;
         std::set<abi_table>                   ctables;
         std::map<std::string, std::string>    rcs;
         std::set<const clang::Type*>          evaluated;
   };

   class eosio_abigen_visitor : public RecursiveASTVisitor<eosio_abigen_visitor>, public generation_utils {
      private:
         bool has_added_clauses = false;
         abigen& ag = abigen::get();
         const clang::CXXRecordDecl* contract_class = NULL;

      public:
         explicit eosio_abigen_visitor(CompilerInstance *CI) {
            get_error_emitter().set_compiler_instance(CI);
         }

         bool shouldVisitTemplateInstantiations() const {
            return true;
         }

         virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl* decl) {
            if (!has_added_clauses) {
               ag.add_clauses(ag.parse_clauses());
               ag.add_contracts(ag.parse_contracts());
               has_added_clauses = true;
            }

            if (decl->isEosioAction() && ag.is_eosio_contract(decl, ag.get_contract_name())) {
               ag.add_struct(decl);
               ag.add_action(decl);
               for (auto param : decl->parameters()) {
                  ag.add_type( param->getType() );
               }
            }

            if (decl->isEosioCall() && ag.is_eosio_contract(decl, ag.get_contract_name())) {
               ag.add_struct(decl);
               ag.add_call(decl);
               for (auto param : decl->parameters()) {
                  ag.add_type( param->getType() );
               }
            }
            return true;
         }
         virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl) {
            if (!has_added_clauses) {
               ag.add_clauses(ag.parse_clauses());
               ag.add_contracts(ag.parse_contracts());
               has_added_clauses = true;
            }
            if ((decl->isEosioAction() || decl->isEosioCall() || decl->isEosioTable()) && ag.is_eosio_contract(decl, ag.get_contract_name())) {
               ag.add_struct(decl);
               if (decl->isEosioAction())
                  ag.add_action(decl);
               if (decl->isEosioCall())
                  ag.add_call(decl);
               if (decl->isEosioTable())
                  ag.add_table(decl);
               for (auto field : decl->fields()) {
                  ag.add_type( field->getType() );
               }
            }
            return true;
         }

         bool is_same_type(const clang::Decl* decl1, const clang::CXXRecordDecl* decl2) const {
            if (!decl1 || !decl2)
               return false;
            if (decl1 == decl2)
               return true;
            
            // checking if declaration is a typedef or using
            if (const clang::TypedefNameDecl* typedef_decl = llvm::dyn_cast<clang::TypedefNameDecl>(decl1)) {
               if (const auto* cur_type = typedef_decl->getUnderlyingType().getTypePtrOrNull()) {
                  if (decl2 == cur_type->getAsCXXRecordDecl()) {
                        return true;
                  }
               }
            }

            return false;
         }
         
         bool defined_in_contract(const clang::ClassTemplateSpecializationDecl* decl) {

            if (!contract_class) {
                  // currently this is unreachable as we do not traverse non-main file translation units
                  CDT_WARN("codegen_warning", decl->getLocation(), "contract class not found: " + ag.get_contract_name());
                  return false;
            }
            
            for (const clang::Decl* cur_decl : contract_class->decls()) {
               if (is_same_type(cur_decl, decl))
                  return true;
            }

            return false;
         }

         virtual bool VisitDecl(clang::Decl* decl) {
            if (const auto* d = dyn_cast<clang::ClassTemplateSpecializationDecl>(decl)) {
               if (d->getName() == "multi_index" || d->getName() == "singleton") {
                  // second template parameter is table type
                  const auto* table_type = d->getTemplateArgs()[1].getAsType().getTypePtr()->getAsCXXRecordDecl();
                  if ((table_type->isEosioTable() && ag.is_eosio_contract(table_type, ag.get_contract_name())) || defined_in_contract(d)) {
                     // first parameter is table name
                     ag.add_table(d->getTemplateArgs()[0].getAsIntegral().getExtValue(), table_type);
                     if (table_type->isEosioTable())
                        ag.add_struct(table_type);
                  }
               }
            }
            return true;
         }
         inline void set_contract_class(const CXXRecordDecl* decl) {
            contract_class = decl;
         }
   };
   class contract_class_finder : public RecursiveASTVisitor<contract_class_finder> {
   private:
      abigen& ag = abigen::get();
      const clang::CXXRecordDecl* contract_class = nullptr;
   public:
      virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl* cxx_decl) {
         if (cxx_decl->isEosioContract()) {
            bool is_eosio_contract = false;
            // on this point it could be just an attribute so let's check base classes
            for (const auto& base : cxx_decl->bases()) {
               if (const clang::Type *base_type = base.getType().getTypePtrOrNull()) {
                  if (const auto* cur_cxx_decl = base_type->getAsCXXRecordDecl()) {
                     if (cur_cxx_decl->getQualifiedNameAsString() == "eosio::contract") {
                        is_eosio_contract = true;
                        break;
                     }
                  }
               }
            }
            if (!is_eosio_contract)
               return true;
            
            auto attr_name = cxx_decl->getEosioContractAttr()->getName();
            auto name = attr_name.empty() ? cxx_decl->getName() : attr_name;
            if (name == llvm::StringRef(ag.get_contract_name())) {
               contract_class = cxx_decl;
               return false;
            }
         }

         return true;
      }
      inline bool contract_found() const {
         return contract_class != nullptr;
      }
      inline const clang::CXXRecordDecl* get_contract() const {
         return contract_class;
      }
   };
   class eosio_abigen_consumer : public ASTConsumer {
      private:
         eosio_abigen_visitor *visitor;
         std::string main_file;
         CompilerInstance* ci;

      public:
         explicit eosio_abigen_consumer(CompilerInstance *CI, std::string file)
            : visitor(new eosio_abigen_visitor(CI)), main_file(file), ci(CI) { }

         virtual void HandleTranslationUnit(ASTContext &Context) {
            auto& src_mgr = Context.getSourceManager();
            auto& f_mgr = src_mgr.getFileManager();
            auto main_fe = f_mgr.getFile(main_file);
            if (main_fe) {
               contract_class_finder cf;
               cf.TraverseDecl(Context.getTranslationUnitDecl());
               if (cf.contract_found())
                  visitor->set_contract_class(cf.get_contract());
               visitor->TraverseDecl(Context.getTranslationUnitDecl());
            }
         }
   };

   class eosio_abigen_frontend_action : public ASTFrontendAction {
      public:
         virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) {
            CI.getPreprocessor().addPPCallbacks(std::make_unique<eosio_ppcallbacks>(CI.getSourceManager(), file.str()));
            return std::make_unique<eosio_abigen_consumer>(&CI, file);
         }
   };
}} // ns eosio::cdt
