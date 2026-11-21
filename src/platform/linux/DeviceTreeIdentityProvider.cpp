// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/DeviceTreeIdentityProvider.hpp>

#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace RetroPixel {
namespace {

bool readProperty(
    const std::string& path,
    std::string& contents)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;

    contents.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());

    return !contents.empty();
}

std::string propertyPath(
    const std::string& root,
    const std::string& property)
{
    if (!root.empty() && root.back() == '/')
        return root + property;

    return root + "/" + property;
}

std::string cleanString(std::string value)
{
    while (!value.empty() &&
           (value.back() == '\0' ||
            value.back() == '\n' ||
            value.back() == '\r' ||
            value.back() == ' ' ||
            value.back() == '\t')) {
        value.pop_back();
    }

    std::size_t first = 0;
    while (first < value.size() &&
           (value[first] == '\n' ||
            value[first] == '\r' ||
            value[first] == ' ' ||
            value[first] == '\t')) {
        ++first;
    }

    return value.substr(first);
}

std::vector<std::string> splitCompatible(const std::string& value)
{
    std::vector<std::string> entries;
    std::size_t start = 0;

    while (start < value.size()) {
        const std::size_t end = value.find('\0', start);
        const std::size_t length =
            end == std::string::npos ? std::string::npos : end - start;

        std::string entry = cleanString(value.substr(start, length));
        if (!entry.empty())
            entries.push_back(std::move(entry));

        if (end == std::string::npos)
            break;

        start = end + 1;
    }

    return entries;
}

} // namespace

DeviceTreeIdentityProvider::DeviceTreeIdentityProvider(
    std::string deviceTreePath)
    : deviceTreePath_(std::move(deviceTreePath))
{
}

DeviceIdentity DeviceTreeIdentityProvider::read()
{
    DeviceIdentity identity;
    std::string value;

    if (readProperty(
            propertyPath(deviceTreePath_, "model"),
            value)) {
        identity.model = cleanString(std::move(value));
        identity.modelValid = !identity.model.empty();
    }

    if (readProperty(
            propertyPath(deviceTreePath_, "compatible"),
            value)) {
        identity.compatible = splitCompatible(value);
        identity.compatibleValid = !identity.compatible.empty();
    }

    return identity;
}

} // namespace RetroPixel
