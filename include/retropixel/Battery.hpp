// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

namespace RetroPixel {

enum class ExternalPowerState {
    Unknown,
    Disconnected,
    Connected
};

enum class ChargingState {
    Unknown,
    Charging,
    Discharging,
    NotCharging,
    Full
};

struct BatteryReading {
    bool capacityValid = false;
    int percent = -1;

    ExternalPowerState externalPower = ExternalPowerState::Unknown;
    ChargingState charging = ChargingState::Unknown;
};

class BatteryProvider {
public:
    virtual ~BatteryProvider() = default;

    virtual BatteryReading read() = 0;
};

} // namespace RetroPixel
