// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

#include <string>

#include <retropixel/DeviceIdentity.hpp>

namespace RetroPixel {

class DeviceTreeIdentityProvider final : public DeviceIdentityProvider {
public:
    explicit DeviceTreeIdentityProvider(
        std::string deviceTreePath = "/proc/device-tree");

    DeviceIdentity read() override;

private:
    std::string deviceTreePath_;
};

} // namespace RetroPixel
