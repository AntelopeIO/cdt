#include "native/eosio/crt.hpp"

#include <cstdio>

eosio::cdt::output_stream std_out;
eosio::cdt::output_stream std_err;

bool ___disable_output = false;
bool ___has_failed = false;
bool ___earlier_unit_test_has_failed = false;

void _prints_l(const char* cstr, uint32_t len, uint8_t which) {
    for (int i=0; i < len; i++) {
        if (which == eosio::cdt::output_stream_kind::std_out)
            std_out.push(cstr[i]);
        else if (which == eosio::cdt::output_stream_kind::std_err)
            std_err.push(cstr[i]);
        if (!___disable_output) {
            std::putc(cstr[i], which == eosio::cdt::output_stream_kind::std_out ? stdout : stderr);
        }
    }
}
void _prints(const char* cstr, uint8_t which) {
    for (int i=0; cstr[i] != '\0'; i++) {
        if (which == eosio::cdt::output_stream_kind::std_out)
            std_out.push(cstr[i]);
        else if (which == eosio::cdt::output_stream_kind::std_err)
            std_err.push(cstr[i]);
        if (!___disable_output)
            std::putc(cstr[i], which == eosio::cdt::output_stream_kind::std_out ? stdout : stderr);
    }
}