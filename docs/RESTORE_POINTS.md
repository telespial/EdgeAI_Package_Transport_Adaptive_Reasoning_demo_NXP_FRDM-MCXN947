# Restore Points

Last updated: 2026-02-22
Project: `EdgeAI_Package_Transport_Anomaly_demo_NXP_FRDM-MCXN947`

## Active Golden
- Golden tag: `GOLDEN-20260222-032039`
- Lock tag: `FAILSAFE-ACTIVE`
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_golden_20260222T032039Z.bin`
- Checksum (sha256): `a27524b891f037aa1c4602cd0d5635b49deae15d221cd619b1e35381abaefc8d`

## Failsafe Active
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`
- Checksum (sha256): `a27524b891f037aa1c4602cd0d5635b49deae15d221cd619b1e35381abaefc8d`

## Notes
- This golden captures the current validated UI/data stack plus buffered LCD fill optimization:
  - shield-IMU-driven sphere gauge (LSM6DSO16IS)
  - live/record modes with PLAY/RECORD controls
  - shield sensor-hub integration for LIS2MDL/LPS22DF/STTS22H plus SHT40
  - shield temperature source priority for terminal TEMP and left temp bargraph
  - AX/AY/AZ/TEMP scope plotting with FIFO persistence
  - chunk-buffered LCD fill rectangle path in `src/par_lcd_s035.c`
- Restore by flashing `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`.
