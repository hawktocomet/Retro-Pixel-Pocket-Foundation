# Wi-Fi Investigation

## Purpose

Identify and reproduce the causes of unreliable Wi-Fi behavior on the FunnyPlaying
Retro Pixel Pocket, then verify any proposed fix with repeatable measurements.

This investigation belongs in Retro-Pixel-Pocket-Foundation because it concerns
hardware, Linux drivers, USB, networking, and system power management.

## Investigation rules

- Begin with read-only inspection and preserve the original configuration.
- Separate **observed**, **hypothesis**, **confirmed**, and **ruled out under
  stated test conditions** findings.
- Record exact commands, test conditions, device/firmware versions, and results.
- Do not treat correlation as proof of causation.
- Do not change configuration, firmware, kernel, modules, drivers, or deployment
  state without explicit user approval.
- For every approved change, record the exact change, rollback procedure, and
  before/after evidence.
- Do not publish credentials, SSIDs, private IP addresses, or other home-network
  identifiers. Keep sensitive raw logs private and publish sanitized evidence.

## Evidence classifications

- **Observed:** Directly present in command output or a reproducible measurement.
- **Hypothesis:** A plausible explanation that has not been demonstrated.
- **Confirmed:** Repeatable evidence establishes the causal mechanism.
- **Ruled out (under stated conditions):** A specified test did not support the
  explanation. This is not proof that the cause is impossible in all conditions.

## Investigation sequence

1. Preserve the read-only hardware, driver, firmware, and network baseline.
2. Establish repeatable latency, loss, and throughput tests.
3. Correlate network interruptions with kernel, USB, and power-management events.
4. Design one-variable-at-a-time tests that distinguish competing explanations.
5. Only after evidence supports a cause, propose a minimal reversible fix.
6. Verify the fix under the same conditions used to establish the baseline.

## Records

- [2026-11-24 baseline](2026-11-24-baseline.md)
- [Test log template](TEST-LOG-TEMPLATE.md)
