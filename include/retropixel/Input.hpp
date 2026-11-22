/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Copyright (C) 2026 hawktocomet */

#pragma once

#include <cstdint>
#include <string>
#include <system_error>
#include <vector>

namespace RetroPixel {

enum class InputReadStatus {
    EventAvailable,
    NoEventAvailable,
    Closed,
    Error
};

struct InputEvent {
    std::uint16_t type = 0;
    std::uint16_t code = 0;
    std::int32_t value = 0;
    std::int64_t timestampSeconds = 0;
    std::int64_t timestampMicroseconds = 0;
};

struct InputDeviceInfo {
    bool nameValid = false;
    std::string name;

    bool hardwareIdValid = false;
    std::uint16_t bus = 0;
    std::uint16_t vendor = 0;
    std::uint16_t product = 0;
    std::uint16_t version = 0;

    bool eventTypesValid = false;
    std::vector<std::uint16_t> eventTypes;

    bool keyCodesValid = false;
    std::vector<std::uint16_t> keyCodes;

    bool absoluteAxesValid = false;
    std::vector<std::uint16_t> absoluteAxes;

    bool relativeAxesValid = false;
    std::vector<std::uint16_t> relativeAxes;

    bool forceFeedbackValid = false;
    bool forceFeedback = false;
};

struct InputReadResult {
    InputReadStatus status = InputReadStatus::Error;
    InputEvent event;
    std::error_code error;
};

class InputDevice {
public:
    virtual ~InputDevice() = default;

    virtual std::error_code open(const std::string& path) = 0;
    virtual void close() noexcept = 0;
    virtual bool isOpen() const noexcept = 0;
    virtual InputDeviceInfo info() const = 0;
    virtual InputReadResult read() = 0;
};

} // namespace RetroPixel
