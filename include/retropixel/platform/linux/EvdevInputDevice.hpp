// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

#include <retropixel/Input.hpp>

#include <string>

namespace RetroPixel {

class EvdevInputDevice final : public InputDevice {
public:
    EvdevInputDevice() = default;
    ~EvdevInputDevice() override;

    EvdevInputDevice(const EvdevInputDevice&) = delete;
    EvdevInputDevice& operator=(const EvdevInputDevice&) = delete;

    std::error_code open(const std::string& path) override;
    void close() noexcept override;
    bool isOpen() const noexcept override;
    InputDeviceInfo info() const override;
    InputReadResult read() override;

private:
    int fileDescriptor_ = -1;
    InputDeviceInfo info_;
};

} // namespace RetroPixel
