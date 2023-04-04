#pragma once

#include "utils.hpp"
#include "file-utils.hpp"
#include "interface.hpp"
#include "intrinsics_setup.hpp"

#include <memory>

#include <dlfcn.h>

namespace eosio { namespace testing { namespace native {
    using apply                 = std::add_pointer_t<void(uint64_t, uint64_t, uint64_t)>;
    using generic_intrinsic     = std::add_pointer_t<void()>;
    using register_intrinsic    = std::add_pointer_t<void (int64_t, const generic_intrinsic&)>;
    using initialize            = std::add_pointer_t<void()>;

    #define REGISTER_LIB_INTRINSIC(ID) \
        exports.register_intrinsic((int64_t)eosio::native::intrinsics::ID, \
                                   reinterpret_cast<generic_intrinsic>(\
                                      get_intrinsic_target<eosio::native::intrinsics::ID, \
                                                           eosio::native::intrinsics::__ ## ID ## _types::res_t, \
                                                           eosio::native::intrinsics::__ ## ID ## _types::deduced_full_ts>\
                                         (eosio::native::intrinsics::__ ## ID ## _types::is)));

    template <typename Fn>
    struct function {
        Fn f = nullptr;

        inline function& operator = ( void* raw ) {
            f = (Fn)raw;
            return *this;
        }

        inline bool operator == (Fn other) {
            return f == other;
        }
        inline bool operator != (Fn other) {
            return !operator==(other);
        }
        template<typename... Args>
        inline auto operator ()(Args... args) {
            return f(args...);
        }
    };

    /// @brief native runner, loads shared object with contract, performs intrinsics setup and exposes interface to execute apply
    struct runner : testing::runner_interface<runner> {
        runner(const std::string& path) {
            load(path);
        }
        void apply(eosio::name receiver, eosio::name code, eosio::name action) {
            exports.apply(receiver.value, code.value, action.value);
        }
        inline object_type get_type() {
            return object_type::shared_object;
        }

        void init() {
            // this call assigns rpc handlers for every intrinsic
            setup_rpc_intrinsics();

            // this macro executes exports.register_intrinsic for every intrinsic
            // we need this because of shared object has its own native library internal variables
            // so this call is to supply current intrinsics pointers to shared object
            INTRINSICS(REGISTER_LIB_INTRINSIC);

            //let library override neccesary intrinsics
            exports.initialize();
        }
    private:
        // if closing handle before main finishes, it gives segmentation fault
        // function with destructor attribute will be executed after main finishes, when static objects destroyed 
        inline static void* so_handle = 0;
        static void close_so_handle(void) __attribute__ ((destructor)) {
            dlclose(so_handle);
        }

        struct exports {
            native::function<native::apply>              apply;
            native::function<native::register_intrinsic> register_intrinsic;
            native::function<native::initialize>         initialize;
        };
        exports      exports;

        void load(const std::string& path) {
            ANTLER_ASSERT(so_handle == nullptr, "object already loaded");

            auto contract_type = utils::get_file_type(path.c_str());
            if ( contract_type != utils::file_type::elf_shared_object ) {
                ANTLER_THROW("not a shared object file: {}", file_type_str(contract_type));
            }

            so_handle = dlopen(path.c_str(), RTLD_LOCAL | RTLD_LAZY);
            if (so_handle == nullptr) {
                ANTLER_THROW("failed to load {} : {}", path, dlerror());
            }

            exports.apply = dlsym(so_handle, "apply");
            if (exports.apply == nullptr) {
                ANTLER_THROW("failed to find `apply`: ", dlerror());
            }

            exports.register_intrinsic = dlsym(so_handle, "register_intrinsic");
            if (exports.register_intrinsic == nullptr) {
                ANTLER_THROW("failed to find `register_intrinsic`: ", dlerror());
            }

            exports.initialize = dlsym(so_handle, "initialize");
            if (exports.apply == nullptr) {
                ANTLER_THROW("failed to find `initialize`: ", dlerror());
            }
        }
    };
}}} // eosio::testing::native