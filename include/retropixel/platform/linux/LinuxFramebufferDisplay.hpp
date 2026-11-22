// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

#include <retropixel/Display.hpp>

#include <string>

namespace RetroPixel {

class LinuxFramebufferDisplay final : public DisplayProvider {
public:
    explicit LinuxFramebufferDisplay(
        std::string device = "/dev/fb0");

    std::error_code read(DisplayInfo& display) override;

private:
    std::string device_;
};

} // namespace RetroPixel
