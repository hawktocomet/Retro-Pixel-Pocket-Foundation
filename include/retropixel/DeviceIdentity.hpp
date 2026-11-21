// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

#include <string>
#include <vector>

namespace RetroPixel {

struct DeviceIdentity {
    bool modelValid = false;
    std::string model;

    bool compatibleValid = false;
    std::vector<std::string> compatible;
};

class DeviceIdentityProvider {
public:
    virtual ~DeviceIdentityProvider() = default;

    virtual DeviceIdentity read() = 0;
};

} // namespace RetroPixel
