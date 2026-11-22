// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/LinuxDeviceCapabilitiesProvider.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

static void writeFile(const fs::path& path, const std::string& contents)
{
    fs::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary);
    file << contents;
}

static bool check(bool condition, const char* description)
{
    if (!condition)
        std::cerr << "FAIL: " << description << '\n';
    else
        std::cout << "PASS: " << description << '\n';

    return condition;
}

int main()
{
    const fs::path root = fs::temp_directory_path() /
        ("retropixel-capabilities-test-" +
         std::to_string(
             std::chrono::steady_clock::now()
                 .time_since_epoch().count()));

    const fs::path sys = root / "sys";
    const fs::path dev = root / "dev";

    bool passed = true;

    // Simulate the interfaces observed on the handheld.
    writeFile(sys / "class/power_supply/battery/capacity", "100\n");
    writeFile(sys / "class/power_supply/battery/status", "Discharging\n");
    writeFile(sys / "class/power_supply/ac/online", "0\n");
    writeFile(sys / "class/power_supply/usb/online", "0\n");

    writeFile(sys / "class/input/js0/device/name", "GO-Super Gamepad\n");
    writeFile(sys / "class/input/event0/device/name", "rk8xx_pwrkey\n");

    writeFile(sys / "class/graphics/fb0/virtual_size", "720,720\n");
    writeFile(sys / "class/graphics/fb0/bits_per_pixel", "32\n");

    writeFile(dev / "input/js0", "");
    writeFile(dev / "event1", "");
    writeFile(dev / "fb0", "");
    writeFile(dev / "snd/pcmC0D0p", "");
    writeFile(dev / "snd/pcmC0D0c", "");

    RetroPixel::LinuxDeviceCapabilitiesProvider provider(
        sys.string(), dev.string());

    auto capabilities = provider.read();

    passed &= check(
        capabilities.batteryTelemetry == RetroPixel::CapabilityState::Available,
        "detects battery telemetry");

    passed &= check(
        capabilities.externalPowerDetection == RetroPixel::CapabilityState::Available,
        "detects external power interfaces");

    passed &= check(
        capabilities.gamepad == RetroPixel::CapabilityState::Available,
        "detects the gamepad");

    passed &= check(
        capabilities.powerButton == RetroPixel::CapabilityState::Available,
        "detects the power button");

    passed &= check(
        capabilities.framebuffer == RetroPixel::CapabilityState::Available &&
        capabilities.displayWidth == 720 &&
        capabilities.displayHeight == 720 &&
        capabilities.displayBitsPerPixel == 32,
        "detects framebuffer and display dimensions");

    passed &= check(
        capabilities.audioPlayback == RetroPixel::CapabilityState::Available,
        "detects audio playback interface");

    passed &= check(
        capabilities.audioCapture == RetroPixel::CapabilityState::Available,
        "detects audio capture interface");

    // Playback and capture must be independent.
    fs::remove(dev / "snd/pcmC0D0c");
    capabilities = provider.read();

    passed &= check(
        capabilities.audioPlayback == RetroPixel::CapabilityState::Available &&
        capabilities.audioCapture == RetroPixel::CapabilityState::Unavailable,
        "reports playback and capture independently");

    // Malformed metadata must not produce false availability.
    writeFile(sys / "class/power_supply/battery/capacity", "not-a-number\n");
    writeFile(sys / "class/graphics/fb0/virtual_size", "720x720\n");

    capabilities = provider.read();

    passed &= check(
        capabilities.batteryTelemetry == RetroPixel::CapabilityState::Unknown,
        "rejects malformed battery capacity");

    writeFile(sys / "class/power_supply/battery/capacity", "100\n");
    writeFile(sys / "class/power_supply/battery/status", "BROKEN\n");
    capabilities = provider.read();

    passed &= check(
        capabilities.batteryTelemetry == RetroPixel::CapabilityState::Unknown,
        "rejects unrecognized battery status");

    writeFile(sys / "class/power_supply/battery/status", "Discharging\n");
    fs::remove(sys / "class/power_supply/usb/online");
    capabilities = provider.read();

    passed &= check(
        capabilities.externalPowerDetection == RetroPixel::CapabilityState::Unknown,
        "reports incomplete external power interfaces as unknown");

    passed &= check(
        capabilities.framebuffer == RetroPixel::CapabilityState::Unknown &&
        capabilities.displayWidth == 0 &&
        capabilities.displayHeight == 0,
        "rejects malformed display dimensions");

    // A completely empty root must not claim hardware availability.
    RetroPixel::LinuxDeviceCapabilitiesProvider emptyProvider(
        (root / "empty-sys").string(),
        (root / "empty-dev").string());

    capabilities = emptyProvider.read();

    passed &= check(
        capabilities.batteryTelemetry == RetroPixel::CapabilityState::Unavailable &&
        capabilities.gamepad == RetroPixel::CapabilityState::Unavailable &&
        capabilities.framebuffer == RetroPixel::CapabilityState::Unavailable &&
        capabilities.audioPlayback == RetroPixel::CapabilityState::Unavailable,
        "reports absent interfaces as unavailable");

    fs::remove_all(root);

    if (!passed) {
        std::cerr << "\nDevice capabilities tests failed.\n";
        return 1;
    }

    std::cout << "\nAll device capabilities tests passed.\n";
    return 0;
}
