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

add_library(retropixel_device_identity
    src/platform/linux/DeviceTreeIdentityProvider.cpp
)

target_include_directories(retropixel_device_identity
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(retropixel_device_identity PUBLIC cxx_std_17)

add_library(retropixel_device_capabilities
    src/platform/linux/LinuxDeviceCapabilitiesProvider.cpp
)

target_include_directories(retropixel_device_capabilities
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(retropixel_device_capabilities PUBLIC cxx_std_17)

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

    add_executable(retropixel_device_identity_tests
        tests/DeviceIdentityProviderTests.cpp
    )

    target_link_libraries(retropixel_device_identity_tests
        PRIVATE retropixel_device_identity
    )

    add_test(
        NAME device_identity
        COMMAND retropixel_device_identity_tests
    )

    add_executable(retropixel_device_capabilities_tests
        tests/DeviceCapabilitiesProviderTests.cpp
    )

    target_link_libraries(retropixel_device_capabilities_tests
        PRIVATE retropixel_device_capabilities
    )

    add_test(
        NAME device_capabilities
        COMMAND retropixel_device_capabilities_tests
    )
endif()

add_library(retropixel_input
    src/platform/linux/EvdevInputDevice.cpp
)

target_include_directories(retropixel_input
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(retropixel_input PUBLIC cxx_std_17)

if(BUILD_TESTING)
    add_executable(retropixel_input_tests
        tests/InputDeviceTests.cpp
    )

    target_link_libraries(retropixel_input_tests
        PRIVATE retropixel_input
    )

    add_test(
        NAME input_device
        COMMAND retropixel_input_tests
    )
endif()

find_package(ALSA REQUIRED)

add_library(retropixel_audio
    src/platform/linux/AlsaAudioOutput.cpp
)

target_include_directories(retropixel_audio
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${ALSA_INCLUDE_DIRS}
)

target_compile_features(retropixel_audio PUBLIC cxx_std_17)
target_link_libraries(retropixel_audio PRIVATE ${ALSA_LIBRARIES})

if(BUILD_TESTING)
    add_executable(retropixel_audio_tests
        tests/AudioOutputTests.cpp
    )

    target_link_libraries(retropixel_audio_tests
        PRIVATE retropixel_audio
    )

    add_test(
        NAME audio_output
        COMMAND retropixel_audio_tests
    )
endif()
