find_package(Boost 1.67 REQUIRED COMPONENTS unit_test_framework)

file(GLOB HEADERS "include/eosio/testing_plugin/*.hpp")
add_library(testing_plugin
            testing_plugin.cpp
            ${HEADERS})

target_link_libraries(
    testing_plugin
    http_plugin
    chain_plugin
    eosio_chain
    eosio_testing
    appbase
    fc)

target_include_directories(
    testing_plugin
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include")
