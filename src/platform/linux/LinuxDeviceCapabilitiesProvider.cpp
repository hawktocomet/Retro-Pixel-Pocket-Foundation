// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/LinuxDeviceCapabilitiesProvider.hpp>

#include <charconv>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <utility>

namespace fs = std::filesystem;

namespace RetroPixel {
namespace {

CapabilityState checkPowerSupply(
    const std::string& sysRoot)
{
    const fs::path battery =
        fs::path(sysRoot) / "class/power_supply/battery";

    std::string capacity;
    std::string status;

    std::ifstream capacityFile(battery / "capacity");
    std::ifstream statusFile(battery / "status");

    if (!capacityFile || !statusFile)
        return CapabilityState::Unavailable;

    std::getline(capacityFile, capacity);
    std::getline(statusFile, status);

    int percent = -1;
    const auto result = std::from_chars(
        capacity.data(),
        capacity.data() + capacity.size(),
        percent);

    if (result.ec != std::errc{} ||
        result.ptr != capacity.data() + capacity.size() ||
        percent < 0 || percent > 100 ||
        (status != "Unknown" &&
         status != "Charging" &&
         status != "Discharging" &&
         status != "Not charging" &&
         status != "Full")) {
        return CapabilityState::Unknown;
    }

    return CapabilityState::Available;
}

CapabilityState checkExternalPower(
    const std::string& sysRoot)
{
    const fs::path power = fs::path(sysRoot) / "class/power_supply";

    int readableSources = 0;

    for (const char* source : {"ac", "usb"}) {
        std::ifstream file(power / source / "online");
        if (!file)
            continue;

        std::string value;
        std::getline(file, value);

        if (value != "0" && value != "1")
            return CapabilityState::Unknown;

        ++readableSources;
    }

    if (readableSources == 0)
        return CapabilityState::Unavailable;

    if (readableSources < 2)
        return CapabilityState::Unknown;

    return CapabilityState::Available;
}

bool readTextFile(const fs::path& path, std::string& value)
{
    std::ifstream file(path);
    if (!file)
        return false;

    std::getline(file, value);
    return !value.empty();
}


CapabilityState checkGamepad(
    const std::string& sysRoot,
    const std::string& devRoot)
{
    const fs::path input = fs::path(sysRoot) / "class/input";
    const fs::path joystick = fs::path(devRoot) / "input";

    std::error_code error;
    if (!fs::exists(joystick / "js0", error) || error)
        return CapabilityState::Unavailable;

    for (const auto& entry : fs::directory_iterator(input, error)) {
        if (error)
            return CapabilityState::Unknown;

        const std::string filename = entry.path().filename().string();
        if (filename.rfind("js", 0) != 0)
            continue;

        std::string name;
        if (readTextFile(entry.path() / "device/name", name))
            return CapabilityState::Available;
    }

    return CapabilityState::Unknown;
}

CapabilityState checkPowerButton(const std::string& sysRoot)
{
    const fs::path input = fs::path(sysRoot) / "class/input";

    std::error_code error;
    if (!fs::exists(input, error) || error)
        return CapabilityState::Unknown;

    for (const auto& entry : fs::directory_iterator(input, error)) {
        if (error)
            return CapabilityState::Unknown;

        const std::string filename = entry.path().filename().string();
        if (filename.rfind("event", 0) != 0)
            continue;

        std::string name;
        if (!readTextFile(entry.path() / "device/name", name))
            continue;

        if (name.find("pwrkey") != std::string::npos ||
            name.find("power") != std::string::npos) {
            return CapabilityState::Available;
        }
    }

    return CapabilityState::Unavailable;
}

bool readPositiveInteger(const fs::path& path, int& value)
{
    std::ifstream file(path);
    if (!file)
        return false;

    std::string text;
    std::getline(file, text);

    int parsed = 0;
    const auto result = std::from_chars(
        text.data(),
        text.data() + text.size(),
        parsed);

    if (result.ec != std::errc{} ||
        result.ptr != text.data() + text.size() ||
        parsed <= 0) {
        return false;
    }

    value = parsed;
    return true;
}

CapabilityState checkFramebuffer(
    const std::string& sysRoot,
    const std::string& devRoot,
    DeviceCapabilities& capabilities)
{
    const fs::path device = fs::path(devRoot) / "fb0";
    const fs::path graphics =
        fs::path(sysRoot) / "class/graphics/fb0";

    std::error_code error;
    if (!fs::exists(device, error) || error)
        return CapabilityState::Unavailable;

    std::ifstream sizeFile(graphics / "virtual_size");
    if (!sizeFile)
        return CapabilityState::Unknown;

    std::string size;
    std::getline(sizeFile, size);

    const std::size_t comma = size.find(',');
    if (comma == std::string::npos)
        return CapabilityState::Unknown;

    const std::string widthText = size.substr(0, comma);
    const std::string heightText = size.substr(comma + 1);

    int width = 0;
    int height = 0;
    const auto widthResult = std::from_chars(
        widthText.data(), widthText.data() + widthText.size(), width);
    const auto heightResult = std::from_chars(
        heightText.data(), heightText.data() + heightText.size(), height);

    int bits = 0;
    if (widthResult.ec != std::errc{} ||
        widthResult.ptr != widthText.data() + widthText.size() ||
        heightResult.ec != std::errc{} ||
        heightResult.ptr != heightText.data() + heightText.size() ||
        width <= 0 || height <= 0 ||
        !readPositiveInteger(graphics / "bits_per_pixel", bits)) {
        return CapabilityState::Unknown;
    }

    capabilities.displayWidth = width;
    capabilities.displayHeight = height;
    capabilities.displayBitsPerPixel = bits;

    return CapabilityState::Available;
}

CapabilityState checkAudio(
    const std::string& devRoot,
    char direction)
{
    const fs::path sound = fs::path(devRoot) / "snd";

    std::error_code error;
    if (!fs::exists(sound, error) || error)
        return CapabilityState::Unavailable;

    for (const auto& entry : fs::directory_iterator(sound, error)) {
        if (error)
            return CapabilityState::Unknown;

        const std::string name = entry.path().filename().string();
        if (name.rfind("pcmC", 0) == 0 &&
            !name.empty() && name.back() == direction) {
            return CapabilityState::Available;
        }
    }

    return CapabilityState::Unavailable;
}

} // namespace

LinuxDeviceCapabilitiesProvider::LinuxDeviceCapabilitiesProvider(
    std::string sysRoot,
    std::string devRoot)
    : sysRoot_(std::move(sysRoot)),
      devRoot_(std::move(devRoot))
{
}

DeviceCapabilities LinuxDeviceCapabilitiesProvider::read()
{
    DeviceCapabilities capabilities;

    capabilities.batteryTelemetry = checkPowerSupply(sysRoot_);
    capabilities.externalPowerDetection = checkExternalPower(sysRoot_);

    capabilities.gamepad = checkGamepad(sysRoot_, devRoot_);
    capabilities.powerButton = checkPowerButton(sysRoot_);

    capabilities.framebuffer =
        checkFramebuffer(sysRoot_, devRoot_, capabilities);

    capabilities.audioPlayback = checkAudio(devRoot_, 'p');
    capabilities.audioCapture = checkAudio(devRoot_, 'c');

    return capabilities;
}

} // namespace RetroPixel
