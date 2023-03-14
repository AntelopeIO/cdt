project(antler-run)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/antler-run.cpp ${CMAKE_BINARY_DIR}/antler-run.cpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp)
find_package(eos-vm)
find_package(cdt_libraries)
find_package(Boost REQUIRED)

set(TOOLS_DIR ${CMAKE_SOURCE_DIR})
set(LIB_DIR ${CMAKE_SOURCE_DIR}/../libraries)

include_directories(${LIB_DIR})

add_tool(antler-run)
target_compile_options(${PROJECT_NAME} PUBLIC -ldl -fpermissive)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,\"\\$ORIGIN/../lib\"")
target_include_directories(${PROJECT_NAME} PUBLIC ${TOOLS_DIR}/external/eos-vm/include
                                                  ${TOOLS_DIR}/external/spdlog/include
                                                  ${LIB_DIR}/eosiolib/contracts
                                                  ${LIB_DIR}/eosiolib/core
                                                  ${LIB_DIR}/eosiolib/capi
                                                  ${LIB_DIR}/eosiolib/native
                                                  ${LIB_DIR}/native
                                                  ${LIB_DIR}/meta_refl/include
                                                  )
target_link_libraries(${PROJECT_NAME} native_eosio native)
