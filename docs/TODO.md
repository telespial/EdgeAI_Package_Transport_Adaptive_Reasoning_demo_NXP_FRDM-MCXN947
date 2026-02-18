# TODO

## Done
- Validate first clean build in `mcuxsdk_ws/build_anomaly`. (done 2026-02-16)
- Flash and verify runtime on FRDM-MCXN947 + LCD-PAR-S035. (done 2026-02-16)
- Publish first golden restore point metadata and failsafe artifacts. (done 2026-02-17)
- Integrate shield sensor-hub path and runtime reads for LIS2MDL/LPS22DF/STTS22H. (done 2026-02-17)
- Prioritize shield temperature for terminal TEMP + left bargraph and align redraw behavior. (done 2026-02-17)
- Refresh golden/failsafe restore point from current validated build. (done 2026-02-17)
- Restore MAG data path with LCD-attached runtime. (done 2026-02-17)

## Active Blockers
- Elapsed runtime clock regressed: no longer matching real time; recent fixes introduced drift/backtracking in deciseconds. Last known-good elapsed behavior needs restore before further timing work. (not done 2026-02-17)
- Run extended long-duration stability burn with all shield sensors enabled and recorder cycling.

## Verification Tasks
- Revert/restore elapsed clock to last known-good build behavior, then re-verify against wall clock over 60s and 5min windows.
- Verify live values move for `MAG`, `BARO`, `SHT`, `STTS` during physical/environment changes.
- Verify REC/PLAY persistence and timeline consistency with shield sensors enabled.
- Capture one golden validation UART log and one UI screenshot set for release evidence.

## Secondary Tasks
- Add optional sensor calibration offsets/scales for MAG and pressure normalization.
