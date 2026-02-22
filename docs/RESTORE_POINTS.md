# Restore Points

Last updated: 2026-02-22
Project: `EdgeAI_Package_Transport_Anomaly_demo_NXP_FRDM-MCXN947`

## Active Golden
- Golden tag: `GOLDEN-20260222-020527`
- Lock tag: `FAILSAFE-ACTIVE`
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_golden_20260222T020527Z.bin`
- Checksum (sha256): `ba344ca335e1c67cbc842425b7bf017d9432ad2ae8ca8b61fc7833d94683fc87`

## Failsafe Active
- Binary: `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`
- Checksum (sha256): `ba344ca335e1c67cbc842425b7bf017d9432ad2ae8ca8b61fc7833d94683fc87`

## Notes
- This golden captures the current validated UI/data stack plus buffered LCD fill optimization:
  - shield-IMU-driven sphere gauge (LSM6DSO16IS)
  - live/record modes with PLAY/RECORD controls
  - shield sensor-hub integration for LIS2MDL/LPS22DF/STTS22H plus SHT40
  - shield temperature source priority for terminal TEMP and left temp bargraph
  - AX/AY/AZ/TEMP scope plotting with FIFO persistence
  - chunk-buffered LCD fill rectangle path in `src/par_lcd_s035.c`
- Restore by flashing `failsafe/edgeai_package_transport_anomaly_demo_cm33_core0_failsafe_active.bin`.
