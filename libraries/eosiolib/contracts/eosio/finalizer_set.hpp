#pragma once

#include "../../capi/eosio/types.h"
#include "../../core/eosio/crypto_bls_ext.hpp"

#include <string>
#include <vector>
#include <array>

namespace eosio {
    namespace internal_use_do_not_use {
        extern "C" {
            __attribute__((eosio_wasm_import))
            void set_finalizers( const char* data, uint32_t len );
        } // extern "C"
    } //internal_use_do_not_use

    struct abi_finalizer_authority {
        std::string           description;
        uint64_t              fweight = 0; // weight that this finalizer's vote has for meeting fthreshold
        std::vector<char>     public_key_g1_affine_le; // Affine little endian

        EOSLIB_SERIALIZE(abi_finalizer_authority, (description)(fweight)(public_key_g1_affine_le));
    };
    struct abi_finalizer_set {
        uint64_t                             fthreshold = 0;
        std::vector<abi_finalizer_authority> finalizers;

        EOSLIB_SERIALIZE(abi_finalizer_set, (fthreshold)(finalizers));
    };

    void set_finalizers( const abi_finalizer_set& fin_set ) {
        for (const auto& finalizer : fin_set.finalizers)
            eosio::check(finalizer.public_key_g1_affine_le.size() == sizeof(bls_g1_affine), "public key has a wrong size" );
        auto packed = eosio::pack(fin_set);
        internal_use_do_not_use::set_finalizers(packed.data(), packed.size());
    }

} //eosio