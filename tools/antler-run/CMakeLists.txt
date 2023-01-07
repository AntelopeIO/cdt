project(antler-run)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/antler-run.cpp ${CMAKE_BINARY_DIR}/antler-run.cpp)
find_package(eos-vm)
find_package(cdt_libraries)

set(TOOLS_DIR ${CMAKE_SOURCE_DIR})
set(LIB_DIR ${CMAKE_SOURCE_DIR}/../libraries)

include_directories(${LIB_DIR})

add_tool(antler-run)
# add_dependencies(${PROJECT_NAME} native_eosio)
target_compile_options(${PROJECT_NAME} PUBLIC -mx32 -ldl -fpermissive)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND APPLE)
    target_compile_options( ${TARGET} PUBLIC -g )
    find_program ( name NAMES "dsymutil" )
    if ( name )
        add_custom_command( TARGET ${TARGET} POST_BUILD COMMAND dsymutil ${BINOUTPUT}/${TARGET} )
    endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,\"\\$ORIGIN/../lib\"")
# find_library(EOSIOLIB native_eosio REQUIRED)
# find_library(CLIB c PATHS ${CMAKE_BINARY_DIR}/../lib REQUIRED)
# find_library(CXXLIB c++ PATHS ${CMAKE_BINARY_DIR}/../lib REQUIRED)


target_include_directories(${PROJECT_NAME} PUBLIC ${TOOLS_DIR}/external/eos-vm/include
                                                  ${LIB_DIR}/eosiolib/contracts
                                                  ${LIB_DIR}/eosiolib/core
                                                  ${LIB_DIR}/eosiolib/capi
                                                  ${LIB_DIR}/eosiolib/native
                                                  ${LIB_DIR}/native
                                                  ${LIB_DIR}/meta_refl/include
                                                  ${LIB_DIR}/boost/include)
target_link_libraries(${PROJECT_NAME} native_eosio)

# target_link_libraries(${PROJECT_NAME} eos-vm EOSIOLIB)