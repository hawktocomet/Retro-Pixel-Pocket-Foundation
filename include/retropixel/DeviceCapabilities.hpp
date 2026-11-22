// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

namespace RetroPixel {

enum class CapabilityState {
    Unknown,
    Unavailable,
    Available
};

struct DeviceCapabilities {
    CapabilityState batteryTelemetry = CapabilityState::Unknown;
    CapabilityState externalPowerDetection = CapabilityState::Unknown;

    CapabilityState gamepad = CapabilityState::Unknown;
    CapabilityState powerButton = CapabilityState::Unknown;

    CapabilityState framebuffer = CapabilityState::Unknown;
    int displayWidth = 0;
    int displayHeight = 0;
    int displayBitsPerPixel = 0;

    CapabilityState audioPlayback = CapabilityState::Unknown;
    CapabilityState audioCapture = CapabilityState::Unknown;
};

class DeviceCapabilitiesProvider {
public:
    virtual ~DeviceCapabilitiesProvider() = default;

    virtual DeviceCapabilities read() = 0;
};

} // namespace RetroPixel
