#pragma once

#include "utils.hpp"
#include "file-utils.hpp"
#include "interface.hpp"

#include <memory>

#include <dlfcn.h>

namespace eosio { namespace native {
    using apply                 = std::add_pointer_t<void(uint64_t, uint64_t, uint64_t)>;
    using generic_intrinsic     = std::add_pointer_t<void()>;
    using register_intrinsic    = std::add_pointer_t<void (uint32_t, const generic_intrinsic&)>;
    using initialize            = std::add_pointer_t<void()>;
    // struct lib_deleter {
    //     void operator ()(void* h) const {
            // int ret = dlclose(h);
            // if (ret != 0) {
            //     auto err = dlerror();
            //     ANTLER_ERROR("error closing shared object: {}", err ? err : "");
            // }
    //     }
    // };
    // using lib_handle            = std::unique_ptr<void*, lib_deleter>;

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

    struct runner : testing::runner_interface<runner> {
        runner(const std::string& path) {
            load(path);
        }
        void apply(eosio::name receiver, eosio::name code, eosio::name action) {
            exports.apply(receiver.value, code.value, action.value);
        }
        inline object_type get_type() {
            return testing::runner_interface<runner>::object_type::shared_object;
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
}} // eosio::native