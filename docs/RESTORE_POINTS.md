# Restore Points

Last updated: 2026-02-18
Project: `EdgeAI_Package_Transport_Anomaly_demo_NXP_FRDM-MCXN947`

## Active Golden
- Golden tag: `GOLDEN-20260218-172744`
- Lock tag: `FAILSAFE-ACTIVE`
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_golden_20260218T172744Z.bin`
- Checksum (sha256): `0e8432b5879efefe2977907e3ac6ec5f778e641f4f3f2d7b79b584eee059f25c`

## Failsafe Active
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`
- Checksum (sha256): `0e8432b5879efefe2977907e3ac6ec5f778e641f4f3f2d7b79b584eee059f25c`

## Notes
- This golden captures the current validated UI/data stack:
  - shield-IMU-driven sphere gauge (LSM6DSO16IS)
  - live/record modes with PLAY/RECORD controls
  - shield sensor-hub integration for LIS2MDL/LPS22DF/STTS22H plus SHT40
  - shield temperature source priority for terminal TEMP and left temp bargraph
  - AX/AY/AZ/TEMP scope plotting with FIFO persistence
- Restore by flashing `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`.
