// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

#include <string>

#include <retropixel/Battery.hpp>

namespace RetroPixel {

class Rk817BatteryProvider final : public BatteryProvider {
public:
    explicit Rk817BatteryProvider(
        std::string devicePath =
            "/sys/devices/platform/ff180000.i2c/i2c-0/0-0020/");

    BatteryReading read() override;

private:
    std::string devicePath_;
};

} // namespace RetroPixel
