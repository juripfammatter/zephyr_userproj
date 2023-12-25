# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(stepper_driver)

# Include headers
target_include_directories(app PRIVATE include)

# shared libraries
set(COMMON_LIBS 
    src/stepper.cpp
    src/controller.cpp
)

# can be set by running west build ... -DTESTS=ON
option(TESTS "Run tests in test/" OFF)

if(TESTS)
    # build only test_main and needed libraries
    target_sources(app PRIVATE test/test_main.cpp ${COMMON_LIBS})
else()
    # build only main and needed libraries
    target_sources(app PRIVATE src/main.cpp ${COMMON_LIBS})
endif()

unset(TESTS CACHE)