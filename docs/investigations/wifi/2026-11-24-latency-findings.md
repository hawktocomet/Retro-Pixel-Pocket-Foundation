# Retro Pixel Pocket Wi-Fi Latency Investigation

**Status:** Investigation paused; no reliable fix identified  
**Updated:** 2026-11-24  
**Hardware:** FunnyPlaying Retro Pixel Pocket  
**Test firmware:** v1.1.3  
**Kernel:** Linux 4.4.189, AArch64  
**Active Wi-Fi driver:** Realtek `8188eu`

## Summary

The Retro Pixel Pocket exhibits severe, highly variable Wi-Fi latency, including when communicating with devices on the local network.

The problem has been observed on the normal home network and reproduced using a nearby phone hotspot with a strong signal. This makes a problem exclusive to the home router, Deco mesh configuration, or home network environment unlikely.

The investigation examined the driver, kernel interfaces, USB transport, power management, packet counters, and available driver source. No definitive root cause or reliable fix was established.

The practical conclusion is that Wi-Fi performance on this hardware/firmware combination is unreliable. Further investigation is paused rather than continuing to spend time on speculative configuration changes.

## Observed behavior

Latency is not simply consistently high. It varies dramatically:

- Some packets return in approximately 5–15 ms.
- Other packets take hundreds of milliseconds.
- Occasional responses take multiple seconds.
- Packet loss has also been observed.

One longer LAN ping sample from the Arch Linux host to the handheld reported:

| Metric | Result |
|---|---:|
| Packets transmitted | 3,482 |
| Packets received | 2,161 |
| Packet loss | 37.938% |
| Minimum RTT | 6.960 ms |
| Average RTT | 252.682 ms |
| Maximum RTT | 8,568.875 ms |

A later sample reported 1.94% packet loss, an average RTT of 333.901 ms, and a maximum of 8,167.679 ms. The variation between samples reinforces that the problem is intermittent and highly unpredictable.

The issue was also reproduced on a phone hotspot with strong signal and the device close to the access point. This is important evidence against attributing the problem solely to the home network.

## Hardware and software investigation

The device reports:

- Linux kernel 4.4.189.
- Active module: `8188eu`.
- Driver version string: `v5.2.2.3_24783.20171030`.
- Wi-Fi interface: `wlan0`.

The candidate source tree contains the same driver version string. This makes it a useful source for investigation, but does not prove that every source file exactly matches the deployed binary.

The separate `r8188eu` staging driver was blacklisted. The device is now using `8188eu` without the staging driver loaded. Keeping the staging driver blacklisted is intentional; it is not needed for the current setup.

### Areas examined

The investigation included:

- TX USB URB submission and completion paths.
- RX USB aggregation and receive processing.
- Driver transmit tasklet and queue handling.
- NAPI and GRO configuration.
- Driver buffer-pool and queue counters.
- RX reorder handling and packet-loss counters.
- Driver power-management behavior.
- ConnMan background scanning configuration.
- Kernel fault logs and Wi-Fi association events.
- The driver's periodic dynamic-management and watchdog work.

The driver source contains multiple mechanisms that could contribute to delays under some conditions. However, no examined mechanism was conclusively connected to the observed multi-second latency.

### Queue and counter observations

During sampled periods of poor latency:

- Transmit buffer pools were not persistently exhausted.
- Hardware transmit queue accounting did not show a persistent backlog.
- Pending RX USB URB counts were stable in the sampled interval.
- Sampled reorder and packet-loss counters did not increase during one observed interval.

These snapshots do not rule out intermittent stalls, USB completion delays, firmware behavior, RF retries, or scheduling problems. They simply did not provide evidence of persistent queue exhaustion during those samples.

### Kernel faults

Historical kernel logs contained faults involving:

- `__cfg80211_connect_result`
- `issue_nulldata`
- `rtw_sta_mstatus_disc_rpt`

There were also Wi-Fi authentication, association, and disconnection events.

These are legitimate signs of driver or wireless-stack instability. However, the faults were historical and were not correlated with the later latency measurements. They cannot currently be identified as the cause of the latency.

## Power-saving experiment

The driver source showed that:

- `rtw_power_mgnt=2` corresponds to `PS_MODE_MAX`.
- This enables the driver's leisure power-save mechanism.
- The cfg80211 power-management setting gates entry into that mechanism.
- `iw dev wlan0 set power_save off` disables that gate.

A controlled test compared 30 pings to the router with power saving on and off:

| Metric | Power save ON | Power save OFF |
|---|---:|---:|
| Packets received | 30/30 | 30/30 |
| Packet loss | 0% | 0% |
| Average RTT | 54.464 ms | 55.037 ms |
| Maximum RTT | 240.328 ms | 194.583 ms |
| Minimum RTT | 5.267 ms | 4.975 ms |

This particular test did not demonstrate a meaningful improvement from disabling power saving. The maximum was somewhat lower, but average latency was essentially unchanged.

Disabling Wi-Fi power saving has nevertheless been the only setting change associated with even marginal perceived improvement during the broader troubleshooting. It remains a reasonable optional workaround to evaluate, not a proven fix. The controlled test restored power saving to its original on state.

## What has not been established

We have not established:

- Whether the primary delay occurs in the Wi-Fi firmware, driver, USB transport, RF retransmission path, or kernel scheduling.
- Whether TX, RX, or both directions are responsible.
- Whether a particular driver defect is responsible.
- Whether a different driver or firmware would materially improve performance.
- Whether a hardware revision would resolve the problem.

The available source and runtime diagnostics were insufficient to prove a single root cause.

## Current configuration and decision

- Keep the `r8188eu` staging driver blacklisted.
- Continue using the active `8188eu` driver.
- Do not make speculative Deco/router configuration changes.
- Treat Wi-Fi latency and reliability as a limitation of the current device/software combination.
- Pause further low-value tuning until new evidence or a materially different driver/firmware option becomes available.

## Conclusion

A substantial investigation has not produced a reliable fix. The problem persists across different access points, including a nearby phone hotspot, and the available driver diagnostics have not identified a conclusive cause.

For practical project purposes, the current Wi-Fi implementation should be treated as unreliable, with potentially severe latency spikes and packet loss. This limitation should be communicated clearly rather than implying that ordinary router tuning or a simple power-saving toggle solves it.

The investigation can be resumed if new hardware, firmware, driver source, or diagnostic evidence becomes available.
