# TODO

## Done
- Validate first clean build in `mcuxsdk_ws/build_anomaly`. (done 2026-02-16)
- Flash and verify runtime on FRDM-MCXN947 + LCD-PAR-S035. (done 2026-02-16)
- Publish first golden restore point metadata and failsafe artifacts. (done 2026-02-17)

## Active Blockers
- Re-test board freeze after closing settings/help popup using hardware stress loop (candidate fix landed 2026-02-17).
- Re-test popup z-order stability under rapid open/close + gyro refresh (candidate fix landed 2026-02-17).
- Stabilize `*` and `?` visibility and touch hit regions.

## Verification Tasks
- Capture UART traces for popup open/close state transitions and attach to `captures/`.
- Run repeated popup stress test (10+ open/close cycles) with no freeze.
- Verify settings values (mode/sensitivity) can be changed and persist in runtime behavior.

## Secondary Tasks
- Confirm visual quality/tint of `docs/spacebox.jpg` background on panel and tune if needed.
