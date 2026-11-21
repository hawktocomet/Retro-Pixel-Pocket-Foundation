// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/rk817/Rk817BatteryProvider.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

static void writeFile(
    const fs::path& path,
    const std::string& contents)
{
    fs::create_directories(path.parent_path());
    std::ofstream file(path);
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
        ("retropixel-battery-test-" +
         std::to_string(
             std::chrono::steady_clock::now()
                 .time_since_epoch().count()));

    const fs::path battery =
        root / "rk817-battery/power_supply/battery";
    const fs::path charger =
        root / "rk817-charger/power_supply";

    fs::create_directories(battery);
    fs::create_directories(charger / "ac");
    fs::create_directories(charger / "usb");

    RetroPixel::Rk817BatteryProvider provider(root.string());
    bool passed = true;

    // Valid capacity, charging status, and USB power.
    writeFile(battery / "capacity", "73\n");
    writeFile(battery / "status", "Charging\n");
    writeFile(charger / "ac/online", "0\n");
    writeFile(charger / "usb/online", "1\n");

    auto reading = provider.read();

    passed &= check(
        reading.capacityValid && reading.percent == 73,
        "reads a valid battery percentage");

    passed &= check(
        reading.charging == RetroPixel::ChargingState::Charging,
        "recognizes charging status");

    passed &= check(
        reading.externalPower == RetroPixel::ExternalPowerState::Connected,
        "detects connected USB power");

    // Malformed capacity must not become zero.
    writeFile(battery / "capacity", "not-a-number\n");
    writeFile(battery / "status", "Not charging\n");
    writeFile(charger / "ac/online", "0\n");
    writeFile(charger / "usb/online", "0\n");

    reading = provider.read();

    passed &= check(
        !reading.capacityValid && reading.percent == -1,
        "rejects malformed capacity");

    passed &= check(
        reading.charging == RetroPixel::ChargingState::NotCharging,
        "recognizes not-charging status");

    passed &= check(
        reading.externalPower == RetroPixel::ExternalPowerState::Disconnected,
        "reports disconnected only when both sources are offline");

    // A missing source makes a negative power conclusion uncertain.
    fs::remove(charger / "usb/online");
    reading = provider.read();

    passed &= check(
        reading.externalPower == RetroPixel::ExternalPowerState::Unknown,
        "reports unknown when a power source cannot be read");

    // An online value other than zero or one is invalid.
    writeFile(charger / "ac/online", "2\n");
    reading = provider.read();

    passed &= check(
        reading.externalPower == RetroPixel::ExternalPowerState::Unknown,
        "rejects invalid external-power values");

    // Out-of-range percentages are invalid.
    writeFile(battery / "capacity", "101\n");
    reading = provider.read();

    passed &= check(
        !reading.capacityValid,
        "rejects percentages above 100");

    // An unrecognized status remains unknown.
    writeFile(battery / "status", "Unknown status\n");
    reading = provider.read();

    passed &= check(
        reading.charging == RetroPixel::ChargingState::Unknown,
        "keeps unrecognized charging status unknown");

    fs::remove_all(root);

    if (!passed) {
        std::cerr << "\nBattery provider tests failed.\n";
        return 1;
    }

    std::cout << "\nAll battery provider tests passed.\n";
    return 0;
}
