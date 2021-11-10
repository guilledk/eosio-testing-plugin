file(GLOB HEADERS "include/eosio/testing_plugin/*.hpp")
add_library(testing_plugin
            testing_plugin.cpp
            ${HEADERS})

target_link_libraries(
    testing_plugin
    chain_plugin
    wallet_plugin
    http_plugin
    eosio_chain
    appbase
    fc)

target_include_directories(
    testing_plugin
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include")
