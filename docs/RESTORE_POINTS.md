# Restore Points

Last updated: 2026-02-17
Project: `EdgeAI_Package_Transport_Anomaly_demo_NXP_FRDM-MCXN947`

## Active Golden
- Golden tag: `GOLDEN-20260217-222344Z`
- Lock tag: `FAILSAFE-ACTIVE`
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_golden_20260217T222344Z.bin`
- Checksum (sha256): `e480f4d64645d3b60d78e2463ae0e3e602f41db7388285d405e2d7f086d17e9f`

## Failsafe Active
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`
- Checksum (sha256): `e480f4d64645d3b60d78e2463ae0e3e602f41db7388285d405e2d7f086d17e9f`

## Notes
- This golden captures the current validated UI/data stack:
  - accelerometer-driven sphere gauge
  - live/record modes with PLAY/RECORD controls
  - real board temperature sensor wired into temp gauge + plots
  - AX/AY/AZ/TEMP scope plotting with FIFO persistence
- Restore by flashing `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`.
