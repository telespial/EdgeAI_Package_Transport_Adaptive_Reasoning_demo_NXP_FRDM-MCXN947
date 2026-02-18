# Restore Points

Last updated: 2026-02-18
Project: `EdgeAI_Package_Transport_Anomaly_demo_NXP_FRDM-MCXN947`

## Active Golden
- Golden tag: `GOLDEN-20260218-175011`
- Lock tag: `FAILSAFE-ACTIVE`
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_golden_20260218T175011Z.bin`
- Checksum (sha256): `e994cd39420d14856ad2fae5159d2b1e58c82cb7ef7cdb887bd7e1fee4e96981`

## Failsafe Active
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`
- Checksum (sha256): `e994cd39420d14856ad2fae5159d2b1e58c82cb7ef7cdb887bd7e1fee4e96981`

## Notes
- This golden captures the current validated UI/data stack:
  - shield-IMU-driven sphere gauge (LSM6DSO16IS)
  - live/record modes with PLAY/RECORD controls
  - shield sensor-hub integration for LIS2MDL/LPS22DF/STTS22H plus SHT40
  - shield temperature source priority for terminal TEMP and left temp bargraph
  - AX/AY/AZ/TEMP scope plotting with FIFO persistence
- Restore by flashing `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`.
