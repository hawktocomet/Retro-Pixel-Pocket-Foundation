// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/LinuxFramebufferDisplay.hpp>

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cerrno>
#include <utility>

namespace RetroPixel {
namespace {

class FileDescriptor {
public:
    explicit FileDescriptor(int fd) : fd_(fd) {}

    ~FileDescriptor()
    {
        if (fd_ >= 0)
            close(fd_);
    }

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    int get() const
    {
        return fd_;
    }

private:
    int fd_;
};

} // namespace

LinuxFramebufferDisplay::LinuxFramebufferDisplay(std::string device)
    : device_(std::move(device))
{
}

std::error_code LinuxFramebufferDisplay::read(DisplayInfo& display)
{
    const int rawFd = open(device_.c_str(), O_RDONLY | O_CLOEXEC);
    if (rawFd < 0)
        return {errno, std::generic_category()};

    const FileDescriptor fd(rawFd);

    fb_var_screeninfo variable {};
    if (ioctl(fd.get(), FBIOGET_VSCREENINFO, &variable) < 0)
        return {errno, std::generic_category()};

    fb_fix_screeninfo fixed {};
    if (ioctl(fd.get(), FBIOGET_FSCREENINFO, &fixed) < 0)
        return {errno, std::generic_category()};

    if (variable.xres == 0 ||
        variable.yres == 0 ||
        variable.xres_virtual == 0 ||
        variable.yres_virtual == 0 ||
        variable.bits_per_pixel == 0 ||
        fixed.line_length == 0) {
        return std::make_error_code(std::errc::invalid_argument);
    }

    DisplayInfo result;

    result.width = variable.xres;
    result.height = variable.yres;
    result.virtualWidth = variable.xres_virtual;
    result.virtualHeight = variable.yres_virtual;
    result.bitsPerPixel = variable.bits_per_pixel;
    result.lineLength = fixed.line_length;
    result.memoryBytes = fixed.smem_len;

    result.red = {variable.red.length, variable.red.offset};
    result.green = {variable.green.length, variable.green.offset};
    result.blue = {variable.blue.length, variable.blue.offset};
    result.transparency = {
        variable.transp.length,
        variable.transp.offset
    };

    display = result;
    return {};
}

} // namespace RetroPixel
