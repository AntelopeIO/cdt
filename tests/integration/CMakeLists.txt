cmake_minimum_required( VERSION 3.5 )

set(EOSIO_VERSION_MIN "3.1")
set(EOSIO_VERSION_SOFT_MAX "4.0")
#set(EOSIO_VERSION_HARD_MAX "")

find_package(leap)
find_package(Protobuf REQUIRED)

find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARY gmp)

### Check the version of eosio
set(VERSION_MATCH_ERROR_MSG "")
EOSIO_CHECK_VERSION(VERSION_OUTPUT "${EOSIO_VERSION}"
                                   "${EOSIO_VERSION_MIN}"
                                   "${EOSIO_VERSION_SOFT_MAX}"
                                   "${EOSIO_VERSION_HARD_MAX}"
                                   VERSION_MATCH_ERROR_MSG)
if(VERSION_OUTPUT STREQUAL "MATCH")
   message(STATUS "Using Leap version ${EOSIO_VERSION}")
elseif(VERSION_OUTPUT STREQUAL "WARN")
   message(WARNING "Using Leap version ${EOSIO_VERSION} even though it exceeds the maximum supported version of ${EOSIO_VERSION_SOFT_MAX}; continuing with configuration, however build may fail.\nIt is recommended to use Leap version ${EOSIO_VERSION_SOFT_MAX}.x")
else() # INVALID OR MISMATCH
   message(FATAL_ERROR "Found Leap version ${EOSIO_VERSION} but it does not satisfy version requirements: ${VERSION_MATCH_ERROR_MSG}\nPlease use Leap version ${EOSIO_VERSION_SOFT_MAX}.x")
endif(VERSION_OUTPUT STREQUAL "MATCH")


enable_testing()

set(TOOLS_DIR ${CMAKE_SOURCE_DIR}/../../tools)
set(TOOLS_BUILD_DIR ${CMAKE_BINARY_DIR}/../../tools)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/../../libraries)

configure_file(${CMAKE_SOURCE_DIR}/contracts.hpp.in ${CMAKE_BINARY_DIR}/contracts.hpp)
configure_file(${TOOLS_DIR}/antler-run/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp)

include_directories(${CMAKE_BINARY_DIR} ${LIB_DIR} ${TOOLS_BUILD_DIR} ${TOOLS_BUILD_DIR}/include ${Protobuf_INCLUDE_DIRS})
link_directories(${TOOLS_BUILD_DIR}/native ${TOOLS_BUILD_DIR}/eosiolib ${TOOLS_BUILD_DIR}/external/antler-interfaces)

file(GLOB INT_TESTS "*.cpp" "*.hpp")

add_eosio_test_executable( integration_tests ${INT_TESTS} )
target_compile_definitions(integration_tests PUBLIC EOSIO_NATIVE EOSIOLIB_DISABLE_MALLOC)
target_compile_options(integration_tests PUBLIC -ldl -fpermissive -ftemplate-backtrace-limit=0)
target_include_directories( integration_tests PUBLIC  ${TOOLS_DIR}/external/spdlog/include
                                                      ${TOOLS_DIR}/external/eos-vm/include
                                                      ${LIB_DIR}/eosiolib/contracts
                                                      ${LIB_DIR}/eosiolib/core
                                                      ${LIB_DIR}/eosiolib/capi
                                                      ${LIB_DIR}/eosiolib/native
                                                      ${LIB_DIR}/native
                                                      ${LIB_DIR}/meta_refl/include)
target_link_libraries( integration_tests ${GMP_LIBRARY} native_eosio native antler_interfaces ${Protobuf_LIBRARIES})

foreach(TEST_SUITE ${INT_TESTS}) # create an independent target for each test suite
  execute_process(COMMAND bash -c "grep -E 'BOOST_AUTO_TEST_SUITE\\s*[(]' ${TEST_SUITE} | grep -vE '//.*BOOST_AUTO_TEST_SUITE\\s*[(]' | cut -d ')' -f 1 | cut -d '(' -f 2" OUTPUT_VARIABLE SUITE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE) # get the test suite name from the *.cpp file
  if (NOT "" STREQUAL "${SUITE_NAME}") # ignore empty lines
    execute_process(COMMAND bash -c "echo ${SUITE_NAME} | sed -e 's/s$//' | sed -e 's/_test$//'" OUTPUT_VARIABLE TRIMMED_SUITE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE) # trim "_test" or "_tests" from the end of ${SUITE_NAME}
    # to run unit_test with all log from blockchain displayed, put "--verbose" after "--", i.e. "unit_test -- --verbose"
    add_test(NAME ${TRIMMED_SUITE_NAME}_integration_test COMMAND integration_tests --run_test=${SUITE_NAME} --report_level=detailed --color_output)
    # build list of tests to run during coverage testing
    if(ctest_tests)
        string(APPEND ctest_tests "|")
    endif()
    string(APPEND ctest_tests ${TRIMMED_SUITE_NAME}_integration_test)
  endif()
endforeach(TEST_SUITE)
set(ctest_tests "'${ctest_tests}' -j8") # surround test list string in apostrophies
