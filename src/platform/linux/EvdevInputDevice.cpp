// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/EvdevInputDevice.hpp>

#include <linux/input.h>

#include <array>
#include <cerrno>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace RetroPixel {
namespace {

template <std::size_t Size>
bool readCapabilities(
    int fd,
    unsigned int eventType,
    std::array<unsigned long, Size>& bits)
{
    bits.fill(0);
    return ::ioctl(fd, EVIOCGBIT(eventType, sizeof(bits)), bits.data()) >= 0;
}

template <std::size_t Size>
std::vector<std::uint16_t> setBits(
    const std::array<unsigned long, Size>& bits,
    unsigned int maximum)
{
    std::vector<std::uint16_t> result;

    for (unsigned int code = 0; code <= maximum; ++code) {
        const unsigned int wordBits = sizeof(unsigned long) * 8;
        const unsigned int word = code / wordBits;
        const unsigned int bit = code % wordBits;

        if (word < Size && (bits[word] & (1UL << bit)) != 0)
            result.push_back(static_cast<std::uint16_t>(code));
    }

    return result;
}

} // namespace

EvdevInputDevice::~EvdevInputDevice()
{
    close();
}

std::error_code EvdevInputDevice::open(const std::string& path)
{
    close();

    const int fd = ::open(
        path.c_str(),
        O_RDONLY | O_NONBLOCK | O_CLOEXEC);

    if (fd < 0)
        return {errno, std::generic_category()};

    fileDescriptor_ = fd;

    std::array<char, 256> name{};
    if (::ioctl(fd, EVIOCGNAME(name.size()), name.data()) >= 0) {
        info_.name = name.data();
        info_.nameValid = true;
    }

    input_id hardwareId{};
    if (::ioctl(fd, EVIOCGID, &hardwareId) >= 0) {
        info_.bus = hardwareId.bustype;
        info_.vendor = hardwareId.vendor;
        info_.product = hardwareId.product;
        info_.version = hardwareId.version;
        info_.hardwareIdValid = true;
    }

    std::array<unsigned long, (EV_MAX + 1 + 8 * sizeof(unsigned long) - 1) /
                                  (8 * sizeof(unsigned long))> eventBits{};

    info_.eventTypesValid = readCapabilities(fd, 0, eventBits);
    if (info_.eventTypesValid)
        info_.eventTypes = setBits(eventBits, EV_MAX);

    constexpr std::size_t keyWords =
        (KEY_MAX + 1 + 8 * sizeof(unsigned long) - 1) /
        (8 * sizeof(unsigned long));
    std::array<unsigned long, keyWords> keyBits{};

    if (readCapabilities(fd, EV_KEY, keyBits)) {
        info_.keyCodes = setBits(keyBits, KEY_MAX);
        info_.keyCodesValid = true;
    }

    constexpr std::size_t absWords =
        (ABS_MAX + 1 + 8 * sizeof(unsigned long) - 1) /
        (8 * sizeof(unsigned long));
    std::array<unsigned long, absWords> absBits{};

    if (readCapabilities(fd, EV_ABS, absBits)) {
        info_.absoluteAxes = setBits(absBits, ABS_MAX);
        info_.absoluteAxesValid = true;
    }

    constexpr std::size_t relWords =
        (REL_MAX + 1 + 8 * sizeof(unsigned long) - 1) /
        (8 * sizeof(unsigned long));
    std::array<unsigned long, relWords> relBits{};

    if (readCapabilities(fd, EV_REL, relBits)) {
        info_.relativeAxes = setBits(relBits, REL_MAX);
        info_.relativeAxesValid = true;
    }

    constexpr std::size_t ffWords =
        (FF_MAX + 1 + 8 * sizeof(unsigned long) - 1) /
        (8 * sizeof(unsigned long));
    std::array<unsigned long, ffWords> ffBits{};

    if (readCapabilities(fd, EV_FF, ffBits)) {
        info_.forceFeedbackValid = true;
        info_.forceFeedback = !setBits(ffBits, FF_MAX).empty();
    }

    return {};
}

void EvdevInputDevice::close() noexcept
{
    if (fileDescriptor_ >= 0)
        ::close(fileDescriptor_);

    fileDescriptor_ = -1;
    info_ = {};
}

bool EvdevInputDevice::isOpen() const noexcept
{
    return fileDescriptor_ >= 0;
}

InputDeviceInfo EvdevInputDevice::info() const
{
    return info_;
}

InputReadResult EvdevInputDevice::read()
{
    if (fileDescriptor_ < 0)
        return {InputReadStatus::Closed, {}, {}};

    input_event raw{};

    for (;;) {
        const ssize_t count = ::read(
            fileDescriptor_, &raw, sizeof(raw));

        if (count == static_cast<ssize_t>(sizeof(raw))) {
            InputEvent event;
            event.type = raw.type;
            event.code = raw.code;
            event.value = raw.value;
            event.timestampSeconds = raw.time.tv_sec;
            event.timestampMicroseconds = raw.time.tv_usec;

            return {InputReadStatus::EventAvailable, event, {}};
        }

        if (count < 0 && errno == EINTR)
            continue;

        if (count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return {InputReadStatus::NoEventAvailable, {}, {}};

        if (count == 0)
            return {InputReadStatus::Closed, {}, {}};

        if (count < 0)
            return {
                InputReadStatus::Error,
                {},
                {errno, std::generic_category()}
            };

        return {
            InputReadStatus::Error,
            {},
            {EIO, std::generic_category()}
        };
    }
}

} // namespace RetroPixel
