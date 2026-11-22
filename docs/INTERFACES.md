# Implemented Interfaces

This document describes the public interfaces currently implemented by
Retro Pixel Pocket Foundation, their Linux providers, and the hardware
interfaces observed during device inspection.

These interfaces are early building blocks, not a complete hardware
abstraction layer or a finished firmware release.

## General design

- Public C++ interfaces are under `include/retropixel/`.
- Linux-specific implementations are under
  `include/retropixel/platform/linux/` and
  `include/retropixel/platform/rk817/`.
- Providers perform synchronous reads; they do not start background
  services or require a frontend.
- Filesystem roots and device paths can be supplied to providers so
  host-side tests can use temporary files.
- Missing or invalid information should not be presented as a confirmed
  hardware state.

The interfaces use C++17.

## Battery

Public API: `retropixel/Battery.hpp`

Linux provider:
`retropixel/platform/rk817/Rk817BatteryProvider.hpp`

`BatteryProvider::read()` returns a `BatteryReading` containing:

- `capacityValid` and `percent`: whether a valid battery percentage was
  read, and its value from 0 to 100.
- `charging`: `Unknown`, `Charging`, `Discharging`, `NotCharging`, or
  `Full`.
- `externalPower`: `Unknown`, `Disconnected`, or `Connected`.

The RK817 provider reads the battery information exposed by the Linux
sysfs tree. Its default device path is:

`/sys/devices/platform/ff180000.i2c/i2c-0/0-0020/`

Capacity is accepted only when it can be parsed as an integer from 0
through 100. Invalid or unavailable capacity is not reported as zero.

External power is reported as disconnected only when both AC and USB
online states can be read and both report offline. A confirmed online
source means external power is connected. Incomplete or invalid readings
remain unknown.

## Device identity

Public API: `retropixel/DeviceIdentity.hpp`

Linux provider:
`retropixel/platform/linux/DeviceTreeIdentityProvider.hpp`

`DeviceIdentityProvider::read()` returns:

- `modelValid` and `model`: the device-tree model string, when available.
- `compatibleValid` and `compatible`: the ordered list of compatible
  identifiers, when available.

The Linux provider reads the `model` and `compatible` properties under
`/proc/device-tree` by default. Device-tree properties are binary data;
the provider handles their NUL termination and the NUL-separated
compatible identifier list.

The device-tree root can be overridden for host-side tests.

## Device capabilities

Public API: `retropixel/DeviceCapabilities.hpp`

Linux provider:
`retropixel/platform/linux/LinuxDeviceCapabilitiesProvider.hpp`

`DeviceCapabilitiesProvider::read()` reports whether selected Linux
interfaces were detected.

Each capability uses one of three states:

- `Available`: the expected interface was detected.
- `Unavailable`: the probe established that the interface was absent.
- `Unknown`: the probe could not establish a reliable result.

Reported capabilities:

| Field | Interface being checked |
|---|---|
| `batteryTelemetry` | Battery capacity and status in power-supply sysfs |
| `externalPowerDetection` | AC and USB online state interfaces |
| `gamepad` | Joystick device and corresponding input name |
| `powerButton` | Input device name identifying a power key |
| `framebuffer` | Framebuffer device and display metadata |
| `audioPlayback` | ALSA playback PCM device node |
| `audioCapture` | ALSA capture PCM device node |

When framebuffer metadata is available, the result also includes
`displayWidth`, `displayHeight`, and `displayBitsPerPixel`.

The provider defaults to `/sys` and `/dev`. Both roots can be overridden
for host-side tests.

Capability detection is interface discovery only. In particular, the
presence of an audio PCM device node does not prove that playback or
capture works. It also does not test application permissions, sound
quality, or actual hardware operation.

## Linux input events

Public API: `retropixel/Input.hpp`

Linux provider:
`retropixel/platform/linux/EvdevInputDevice.hpp`

`EvdevInputDevice` reads raw Linux evdev events from one configurable
input-device path. It does not map events to frontend-specific buttons
or start a background service.

The device is opened read-only and nonblocking. `read()` returns one of
these statuses:

- `EventAvailable`: one raw event was read.
- `NoEventAvailable`: no event is currently waiting.
- `Closed`: the device is not open or the input source reached EOF.
- `Error`: the read failed; an error code describes the failure.

Each event includes its Linux event type, code, value, and timestamp.

`InputDevice::info()` reports metadata with separate validity flags,
including the device name, hardware bus/vendor/product/version IDs,
event types, key codes, absolute and relative axes, and force-feedback
capability. Unsupported or unavailable metadata does not prevent the
device from opening.

The provider uses Linux evdev (`/dev/input/event*`), not the legacy
joystick interface (`/dev/input/js*`). Access is subject to normal
device-node permissions; the application must have permission to read
the selected device.

Host-side tests exercise event translation, open/close behavior, and
unavailable metadata using a regular file as a simulated event source.
Live-device behavior and nonblocking reads still require testing on the
handheld.

## Hardware observations

The following interfaces were observed on the inspected handheld running
its stock firmware. These are observations of that device and firmware,
not guarantees for every hardware revision or firmware version.

| Area | Observed interface or value |
|---|---|
| Device model | `Anbernic RG351MP` |
| Device-tree compatible identifiers | `rockchip,rk3326-rg351mp-linux`, `rockchip,rk3326` |
| Battery | `/sys/class/power_supply/battery` |
| External power | `/sys/class/power_supply/ac` and `/sys/class/power_supply/usb` |
| Input | `GO-Super Gamepad`, joystick `js0`, and power-key input |
| Display | `/dev/fb0`, 720 × 720, 32 bits per pixel |
| Audio | RK817 codec; ALSA playback and capture PCM device nodes |

The observed audio interfaces establish that the kernel exposes audio
devices. Actual playback and capture still require functional testing.

## Verification

The providers have host-side tests using simulated filesystem trees and regular files. They cover successful reads, event translation, missing interfaces, malformed values, and unknown states.

Configure, build, and run the tests:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Passing host-side tests verifies provider behavior against simulated inputs. It does not, by itself, verify operation on the physical device.

## Not yet provided

These interfaces do not currently provide:

- Display rendering or mode setting.
- Frontend-specific gamepad button mapping.
- Audio playback, recording, or volume control.
- Storage discovery or mount management.
- Power-button event handling or suspend/resume control.
- A background service, IPC protocol, or frontend-specific behavior.

Those capabilities should be added when there is a demonstrated need and a clear, reusable interface to support them.
