# Status

Last updated: 2026-02-17

- Project build target: `edgeai_package_transport_anomaly_demo` (`frdmmcxn947/cm33_core0`)
- Current build status: PASS (`mcuxsdk_ws/build_adaptive_reasoning`)
- Flash status: not run in this restore-point cut (last known PASS on FRDM-MCXN947 probe `UYLKOJI11H2B3`)

## Golden Baseline (Active)
- Golden tag: `GOLDEN-20260217-222344Z`
- Active failsafe: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`
- Golden image: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_golden_20260217T222344Z.bin`
- sha256: `e480f4d64645d3b60d78e2463ae0e3e602f41db7388285d405e2d7f086d17e9f`

## Frozen Features in Golden
- Accelerometer sphere gauge with fast redraw.
- Scope UI with PLAY (left) and RECORD (right) controls.
- Persistent AX/AY/AZ/TEMP traces.
- Board digital temperature sensor wired to temp gauge + trace.

## Current Runtime Validation
- On-board temperature sensor U6 (`P3T1755DP`) is now reading via I3C1 dynamic address flow.
- UART boot log confirms:
  - `TEMP ready dyn=0x08 bus=i3c T=24C`
- Prior stuck `99C` behavior was traced to a decode scale bug and is fixed.
- Temperature display now shows both units side-by-side (`C` and `F`) in terminal and left bar label.
- Boot-time raw-byte validation:
  - `TEMP init raw=0x18 0xf0 -> 24C/75F`
- Temperature display now supports one decimal place in both units (live sensor path uses raw-byte-derived tenths).
- Accelerometer sphere and `ACCEL VECTOR` title are now re-centered together after right shift (+5% screen width).
- Latest layout tweak: sphere/text moved left by 2.5% of screen width from that prior position.
- Horizon line roll response is calibrated to track sphere movement more accurately, reducing perceived mismatch.
- Gyro fast path is dynamic-only for responsive refresh; dynamic overlays are circle-clipped to keep all moving pixels inside the sphere and remove square-edge artifacts.
- Battery indicator is pinned to demo value `82%`.
- External-flash recorder is a robust one-record-per-page FIFO with gyro-ready sample fields (`gx/gy/gz`) and decimal temperature (`temp_c10`).
- External-flash recorder uses the upper half of NOR for recording, reserving one sector for metadata/generation.
- Current retention (FRDM-MCXN947 W25Q64 geometry, 10 Hz): ~27.3 minutes before FIFO overwrite.
- Recorder state now reconstructs from flash on boot (recordings persist across reboot).
- Timeline mode now defaults to PLAY on boot and auto-starts external-flash playback when recorded data is available.
- PLAY button now rewinds playback to beginning of current recording window on each press.
- RECORD now requires confirmation; YES clears recorder flash region first, then recording starts.
- RECORD confirm YES is non-blocking (fast logical clear), and confirm popup is forced topmost during fast refresh with close-transition cleanup redraw.
- Scope graph now includes a vertical playhead line tied to recorder state (green in PLAY, red in RECORD) for REC/PLAY verification.
- Battery fill/text and temp-related dynamic visuals are now forced to redraw reliably after modal transitions.
- REC/PLAY stepping now runs at 10 Hz; display refresh also runs at 10 Hz for smoother playback.
- Scope graph sampling cadence is now aligned to 10 Hz (`SCOPE_FAST_STEP_US=100000`) to keep plotted timeline pace matched to recorded/playback data pace.
- PLAY mode scope plotting now advances on playhead movement (timeline-relative), preventing rapid full-window cycling that looked faster than recorded timeline pace.
