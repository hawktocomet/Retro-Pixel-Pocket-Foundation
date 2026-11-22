// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

#include <cstdint>
#include <system_error>

namespace RetroPixel {

struct DisplayChannel {
    std::uint32_t length = 0;
    std::uint32_t offset = 0;
};

struct DisplayInfo {
    std::uint32_t width = 0;
    std::uint32_t height = 0;

    std::uint32_t virtualWidth = 0;
    std::uint32_t virtualHeight = 0;

    std::uint32_t bitsPerPixel = 0;
    std::uint32_t lineLength = 0;
    std::uint64_t memoryBytes = 0;

    DisplayChannel red;
    DisplayChannel green;
    DisplayChannel blue;
    DisplayChannel transparency;
};

class DisplayProvider {
public:
    virtual ~DisplayProvider() = default;

    virtual std::error_code read(DisplayInfo& display) = 0;
};

} // namespace RetroPixel
