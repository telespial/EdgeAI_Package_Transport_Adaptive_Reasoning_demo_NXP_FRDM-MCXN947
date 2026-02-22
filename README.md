# EdgeAI Package Transport Anomaly Demo (FRDM-MCXN947)

**Work in progress:** this repository is under active development and is not complete code.

Standalone Edge AI anomaly-detection firmware demo for package-transport telemetry on **FRDM-MCXN947 + LCD-PAR-S035**.

## Scope
- On-device anomaly scoring and risk-state classification.
- Live dashboard rendering (gauges + timeline + status banner).
- Replay-capable telemetry pipeline for repeatable validation.
- Build/flash workflow using project-local scripts.

## Quickstart
```bash
./tools/bootstrap_ubuntu_user.sh
./tools/setup_mcuxsdk_ws.sh
BUILD_DIR=mcuxsdk_ws/build_anomaly ./tools/build_frdmmcxn947.sh debug
BUILD_DIR=mcuxsdk_ws/build_anomaly ./tools/flash_frdmmcxn947.sh
```

## Repository Layout
- `src/`: firmware source
- `sdk_example/`: MCUX SDK overlay wrapper
- `tools/`: setup/build/flash scripts
- `docs/`: runbook, state, full design/process docs, restore and logs
- `data/`: replay telemetry assets
- `failsafe/`: pinned recovery binaries/checksums

## EIL Model Profile Import
Use an exported EIL `model.config.json` to drive runtime thresholds/weights in firmware:

```bash
python3 tools/import_eil_profile.py \
  --model /path/to/model.config.json \
  --out src/eil_profile_generated.h
```

Current integration maps:
- `alertThresholds.warn/fail` -> runtime `WARNING`/`FAULT` transitions
- input `weight` for `accel_x_g`, `accel_y_g`, `accel_z_g`, `temp_c` -> weighted anomaly score

### On-Board Train -> Play -> Live
- No CSV round-trip is required for baseline training.
- Use device controls:
  - `AI_MODE TRAIN` + `RECORD`: collect on-device baseline/training frames.
  - stop record -> `PLAY`: firmware continues fitting from replay if training window is incomplete.
  - when fit reaches ready state, firmware auto-switches to live monitor mode.
