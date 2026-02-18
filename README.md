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
