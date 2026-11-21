// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/DeviceTreeIdentityProvider.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;
using namespace std::string_literals;

static void writeFile(
    const fs::path& path,
    const std::string& contents)
{
    fs::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary);
    file.write(contents.data(), static_cast<std::streamsize>(contents.size()));
}

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
    const fs::path root = fs::temp_directory_path() /
        ("retropixel-identity-test-" +
         std::to_string(
             std::chrono::steady_clock::now()
                 .time_since_epoch().count()));

    bool passed = true;

    // Device-tree strings are null-terminated.
    writeFile(root / "model", "Retro Pixel Pocket\0"s);
    writeFile(
        root / "compatible",
        "rockchip,rk3326-rg351mp-linux\0rockchip,rk3326\0\0"s);

    RetroPixel::DeviceTreeIdentityProvider provider(root.string());
    auto identity = provider.read();

    passed &= check(
        identity.modelValid && identity.model == "Retro Pixel Pocket",
        "reads the device model");

    passed &= check(
        identity.compatibleValid && identity.compatible.size() == 2,
        "reads multiple compatible identifiers");

    passed &= check(
        identity.compatible.size() == 2 &&
        identity.compatible[0] == "rockchip,rk3326-rg351mp-linux" &&
        identity.compatible[1] == "rockchip,rk3326",
        "preserves compatible identifier order");

    // Missing properties must remain explicitly unknown.
    fs::remove(root / "model");
    fs::remove(root / "compatible");

    identity = provider.read();

    passed &= check(
        !identity.modelValid && identity.model.empty(),
        "reports a missing model as invalid");

    passed &= check(
        !identity.compatibleValid && identity.compatible.empty(),
        "reports missing compatibility data as invalid");

    // An empty property is not a valid identity.
    writeFile(root / "model", "");
    writeFile(root / "compatible", "\0\0"s);

    identity = provider.read();

    passed &= check(
        !identity.modelValid,
        "rejects an empty model property");

    passed &= check(
        !identity.compatibleValid,
        "rejects an empty compatible property");

    fs::remove_all(root);

    if (!passed) {
        std::cerr << "\nDevice identity tests failed.\n";
        return 1;
    }

    std::cout << "\nAll device identity tests passed.\n";
    return 0;
}
