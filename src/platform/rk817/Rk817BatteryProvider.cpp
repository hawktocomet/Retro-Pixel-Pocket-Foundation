// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/rk817/Rk817BatteryProvider.hpp>

#include <charconv>
#include <fstream>
#include <string>
#include <system_error>
#include <utility>

namespace RetroPixel {
namespace {

bool readText(const std::string& path, std::string& value)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    std::string line;
    if (!std::getline(file, line))
        return false;

    const auto first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return false;

    const auto last = line.find_last_not_of(" \t\r\n");
    value = line.substr(first, last - first + 1);
    return true;
}

bool parseInteger(const std::string& text, int& value)
{
    int parsed = 0;
    const char* begin = text.data();
    const char* end = begin + text.size();

    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc{} || result.ptr != end)
        return false;

    value = parsed;
    return true;
}

std::string deviceFile(
    const std::string& base,
    const std::string& relativePath)
{
    if (!base.empty() && base.back() == '/')
        return base + relativePath;

    return base + "/" + relativePath;
}

} // namespace

Rk817BatteryProvider::Rk817BatteryProvider(std::string devicePath)
    : devicePath_(std::move(devicePath))
{
}

BatteryReading Rk817BatteryProvider::read()
{
    BatteryReading reading;
    std::string value;
    int parsed = 0;

    const std::string capacityPath = deviceFile(
        devicePath_,
        "rk817-battery/power_supply/battery/capacity");

    if (readText(capacityPath, value) &&
        parseInteger(value, parsed) &&
        parsed >= 0 && parsed <= 100) {
        reading.capacityValid = true;
        reading.percent = parsed;
    }

    const std::string statusPath = deviceFile(
        devicePath_,
        "rk817-battery/power_supply/battery/status");

    if (readText(statusPath, value)) {
        if (value == "Charging")
            reading.charging = ChargingState::Charging;
        else if (value == "Discharging")
            reading.charging = ChargingState::Discharging;
        else if (value == "Not charging")
            reading.charging = ChargingState::NotCharging;
        else if (value == "Full")
            reading.charging = ChargingState::Full;
    }

    const std::string acPath = deviceFile(
        devicePath_,
        "rk817-charger/power_supply/ac/online");

    const std::string usbPath = deviceFile(
        devicePath_,
        "rk817-charger/power_supply/usb/online");

    int acOnline = 0;
    int usbOnline = 0;

    const bool acValid =
        readText(acPath, value) &&
        parseInteger(value, acOnline) &&
        (acOnline == 0 || acOnline == 1);

    const bool usbValid =
        readText(usbPath, value) &&
        parseInteger(value, usbOnline) &&
        (usbOnline == 0 || usbOnline == 1);

    if ((acValid && acOnline == 1) ||
        (usbValid && usbOnline == 1)) {
        reading.externalPower = ExternalPowerState::Connected;
    } else if (acValid && usbValid) {
        reading.externalPower = ExternalPowerState::Disconnected;
    }

    return reading;
}

} // namespace RetroPixel
