// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/LinuxFramebufferDisplay.hpp>

#include <iostream>

static bool check(bool condition, const char* description)
{
    if (!condition)
        std::cerr << "FAIL: " << description << '\n';
    else
        std::cout << "PASS: " << description << '\n';

    return condition;
}

int main()
{
    bool passed = true;

    RetroPixel::LinuxFramebufferDisplay missing("/no/such/framebuffer");

    RetroPixel::DisplayInfo display;
    display.width = 123;
    display.height = 456;

    const auto error = missing.read(display);

    passed &= check(
        error == std::errc::no_such_file_or_directory,
        "reports a missing framebuffer device");

    passed &= check(
        display.width == 123 && display.height == 456,
        "leaves display information unchanged after failure");

    if (!passed) {
        std::cerr << "\nDisplay provider tests failed.\n";
        return 1;
    }

    std::cout << "\nAll display provider tests passed.\n";
    return 0;
}
