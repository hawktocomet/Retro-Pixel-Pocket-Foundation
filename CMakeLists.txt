# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 hawktocomet

cmake_minimum_required(VERSION 3.16)

project(RetroPixelFoundation
    VERSION 0.1.0
    DESCRIPTION "Reusable platform foundation for the Retro Pixel Pocket"
    LANGUAGES CXX
)

add_library(retropixel_battery
    src/platform/rk817/Rk817BatteryProvider.cpp
)

target_include_directories(retropixel_battery
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(retropixel_battery PUBLIC cxx_std_17)

include(CTest)

if(BUILD_TESTING)
    add_executable(retropixel_battery_tests
        tests/BatteryProviderTests.cpp
    )

    target_link_libraries(retropixel_battery_tests
        PRIVATE retropixel_battery
    )

    add_test(
        NAME battery_provider
        COMMAND retropixel_battery_tests
    )
endif()
