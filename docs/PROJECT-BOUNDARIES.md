# Project Boundaries

## Purpose

Retro Pixel Pocket Foundation and Retro KiddOS are separate projects with different responsibilities.

The Foundation provides the reusable device platform. KiddOS provides one possible user experience built on that platform.

This separation allows developers to create their own interfaces and applications without needing to adopt KiddOS.

## Retro Pixel Pocket Foundation

The Foundation owns the hardware-facing and system-level components required to operate the Retro Pixel Pocket.

Its responsibilities include:

- Bootloader, boot configuration, and boot process
- Linux kernel, kernel configuration, and device tree
- Hardware drivers and device support
- Base operating system and system initialization
- Hardware-facing system services
- Shared runtime capabilities and documented application interfaces
- Build system, toolchains, and dependency provisioning
- Firmware image generation and installation
- Component provenance, licensing, and redistribution documentation
- Developer documentation and reproducible build instructions

The Foundation should expose stable, documented interfaces that independent applications and frontends can use.

It must not require KiddOS to build, boot, or operate.

## Retro KiddOS

[Retro KiddOS](https://github.com/hawktocomet/Retro-KiddOS) owns the child-friendly application and frontend experience.

Its responsibilities include:

- User interface and navigation
- Game library presentation and organization
- Game selection and launching experience
- Child profiles and preferences
- Favorites and recently played games
- Game curation and age-appropriate organization
- Parent-facing controls and management workflows
- Application-specific settings and policies

KiddOS may use Foundation-provided services and interfaces, but those services must not contain KiddOS-specific behavior or assumptions.

## Independence

The projects must remain independently useful.

The Foundation must be usable with other frontends and applications.

KiddOS should be able to run on compatible stock firmware or another implementation of the interfaces it requires, where supported.

Neither project should require the other's source tree to build its own deliverables.

## Interface ownership

The Foundation owns the implementation and documentation of shared hardware and system interfaces.

KiddOS owns its application-specific use of those interfaces.

Interfaces should be designed around general device capabilities rather than the needs of one particular frontend.

Examples of Foundation-level capabilities include:

- Battery and external power information
- Display configuration and capabilities
- Input device access
- Audio capabilities
- Storage and filesystem access
- System information and device status
- Power management

Examples of KiddOS-level behavior include:

- Displaying a battery icon
- Deciding which games appear in a carousel
- Remembering a child's favorite games
- Choosing what happens when a child exits a game

## Development principles

1. Keep Foundation independent of frontend-specific code and concepts.
2. Prefer reusable system interfaces over application-specific shortcuts.
3. Keep application behavior in the application that owns it.
4. Document public interfaces and compatibility expectations.
5. Preserve compatibility with existing stock firmware integrations where practical.
6. Avoid introducing services, abstractions, or infrastructure without a demonstrated need.
7. Verify licensing and redistribution rights before incorporating third-party material.
8. Keep both projects buildable and maintainable independently.

## Long-term direction

The Foundation should become a transparent, open-source, reproducible platform for the Retro Pixel Pocket.

KiddOS should serve as a practical reference implementation demonstrating how an independent frontend can use that platform.

Other developers should be able to replace KiddOS with their own software without needing to fork or redesign the Foundation.
