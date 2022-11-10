cmake_minimum_required(VERSION 3.5)

project(cdt_libraries)

list(APPEND CMAKE_MODULE_PATH ${CDT_BIN})
include(CDTMacros)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -no-missing-ricardian-clause")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

add_subdirectory(libc)
add_subdirectory(libc++)
add_subdirectory(eosiolib)
add_subdirectory(boost)
add_subdirectory(rt)

if (ENABLE_NATIVE_COMPILER)
   add_subdirectory(native)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/meta_refl/include/bluegrass DESTINATION ${BASE_BINARY_DIR}/include)
