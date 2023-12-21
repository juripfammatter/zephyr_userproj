# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(unit_test)

# can be set by running west build ... -DTESTS=ON
option(TESTS "Run tests in test/" OFF)
if(TESTS)
    # build only test_main
    target_sources(app PRIVATE test/test_main.cpp)
else()
    # build only main
    target_sources(app PRIVATE src/main.cpp)
endif()
unset(TESTS CACHE)