#include "native/eosio/crt.hpp"
#include "native/eosio/intrinsics.hpp"

#include <cstdio>

eosio::cdt::output_stream std_out;
eosio::cdt::output_stream std_err;

bool ___disable_output = false;
bool ___has_failed = false;
bool ___earlier_unit_test_has_failed = false;
static jmp_buf env;
static jmp_buf test_env;
jmp_buf* ___env_ptr = &env;

jmp_buf* __get_jmp_buf() {
    return ___env_ptr;
}
void __set_env_test() {
    ___env_ptr = &test_env;
}
void __reset_env() {
    ___env_ptr = &env;
}

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


template<eosio::native::intrinsics::intrinsic_name ID, typename Fn, typename Ret, typename TupleArgs, size_t... Is>
void set_intrinsic(Fn fn, std::index_sequence<Is...>) {
    eosio::native::intrinsics::set_intrinsic<ID>(reinterpret_cast<Ret(*)(typename std::tuple_element<Is, TupleArgs>::type...)>(fn));
}

#define REGISTER_INTRINSIC(ID) \
    case eosio::native::intrinsics::ID: \
    set_intrinsic<eosio::native::intrinsics::ID, \
                  decltype(fn), \
                  eosio::native::intrinsics::__ ## ID ## _types::res_t, \
                  eosio::native::intrinsics::__ ## ID ## _types::deduced_full_ts> \
        (fn, eosio::native::intrinsics::__ ## ID ## _types::is); \
    break;


void register_intrinsic(int64_t id, void(*fn)()) {
    switch(id) {
        INTRINSICS(REGISTER_INTRINSIC);
    }
}