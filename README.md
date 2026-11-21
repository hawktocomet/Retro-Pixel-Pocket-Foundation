# Retro Pixel Pocket Foundation

An open-source, reusable firmware foundation for the FunnyPlaying Retro Pixel Pocket.

The goal is to make the hardware understandable, customizable, and reproducible: developers should be able to start with documented public sources, follow the build instructions, and produce a working firmware image without relying on private archives, undocumented files, or the original developer's machine.

## Project goals

- Provide a reusable foundation independent of any particular frontend.
- Document the hardware, boot process, operating system, drivers, and system services.
- Publish original source code, build scripts, configuration, patches, and documentation.
- Use publicly obtainable dependencies with clear licensing and provenance.
- Make firmware builds reproducible and approachable for other developers.
- Prefer upstream projects and upstream contributions where practical.
- Clearly identify proprietary, unavailable, or redistribution-restricted components.

## Project boundaries

This repository owns the device foundation:

- Bootloader and boot configuration
- Linux kernel and device tree
- Hardware support and drivers
- Base operating system and system services
- Runtime and application integration interfaces
- Build system, dependency provisioning, and image generation
- Licensing, provenance, and developer documentation

It does not own a particular user interface, game library, child profile system, or frontend experience.

[Retro KiddOS](https://github.com/hawktocomet/Retro-KiddOS) is a separate project. It is intended to be one replaceable frontend that can use the Foundation's documented interfaces. The Foundation must remain useful without KiddOS.

## Open-source commitment

We aim to make this project fully open source to the greatest extent legally and technically possible.

Original project code and tooling will be publicly available under clearly identified licenses. Third-party components will retain their respective licenses and attribution requirements. We will document components that cannot legally be redistributed or whose licensing remains unclear rather than silently depending on them.

Open source also means making the build process transparent: a clean checkout and documented prerequisites should be sufficient to reproduce the work.

## Current status

The project is being established as a separate repository.

Prior research has already examined the stock firmware, boot process, Linux kernel, bootloader, device tree, hardware services, and available build tooling. That work will be incorporated here incrementally, with recovered artifacts and technical conclusions verified before being treated as authoritative.

A complete, independently reproducible firmware build has not yet been established.

## Repository contents

Documentation, source code, build tooling, configuration, patches, and other project-owned materials will be organized here as the foundation develops.

See [CONTRIBUTING.md](CONTRIBUTING.md) for development principles and [docs/PROJECT-BOUNDARIES.md](docs/PROJECT-BOUNDARIES.md) for ownership boundaries.

## License

Original Foundation code is licensed under GNU GPL v3 or later. Third-party components retain their own licenses and attribution requirements. Do not assume that the license of one component applies to the entire firmware.

## Battery interface

The first reusable hardware interface is the C++ battery API in
`include/retropixel/Battery.hpp`.

The initial RK817 provider reads battery capacity, charging status, and
external power state from the device's Linux sysfs interface. Its sysfs
root can be supplied to the provider, allowing host-side tests to use
temporary files instead of physical hardware.

The API distinguishes unknown readings from confirmed states. Invalid or
unavailable capacity data is not reported as zero, and external power is
reported as disconnected only when both AC and USB status can be read and
both report offline.

### Build and test

Requirements: CMake 3.16 or newer and a C++17 compiler.

Configure and build:

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel

Run tests:

    ctest --test-dir build --output-on-failure

The battery provider tests run on the host and do not require a Retro Pixel
Pocket device.

This is an initial interface and RK817 implementation, not yet a complete
firmware build or a hardware-validated release.
