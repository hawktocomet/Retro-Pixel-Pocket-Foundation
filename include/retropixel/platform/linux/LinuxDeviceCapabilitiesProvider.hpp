// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

#include <string>

#include <retropixel/DeviceCapabilities.hpp>

namespace RetroPixel {

class LinuxDeviceCapabilitiesProvider final
    : public DeviceCapabilitiesProvider {
public:
    explicit LinuxDeviceCapabilitiesProvider(
        std::string sysRoot = "/sys",
        std::string devRoot = "/dev");

    DeviceCapabilities read() override;

private:
    std::string sysRoot_;
    std::string devRoot_;
};

} // namespace RetroPixel
