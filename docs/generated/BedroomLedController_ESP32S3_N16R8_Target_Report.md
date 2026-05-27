# Bedroom LED Controller ESP32-S3-N16R8 target migration

## Target

- Primary target: ESP32-S3-N16R8.
- Arduino CLI FQBN: `esp32:esp32:esp32s3`.
- Compile options: `FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB`.
- The D1 mini / ESP8266 path remains conditionally supported unless a future pass explicitly removes it.

## Firmware changes

- Board identity now reports `ESP32-S3` when `CONFIG_IDF_TARGET_ESP32S3` is defined.
- Diagnostics/state/API docs expose the ESP32-S3 compile target and PSRAM presence/size fields.
- Existing ESP8266 conditional includes and server declarations remain in place for compatibility.

## Verification

- Python tests: passed.
  - Command: `python -m unittest discover -s tests -v`
  - Result: 155 tests passed.
- ESP32-S3 compile: passed.
  - Command: `arduino-cli compile --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB" firmware\BedroomLedController`
  - Program storage: 1,435,335 / 3,145,728 bytes (45%).
  - Dynamic memory: 67,496 / 327,680 bytes (20%).
- D1 mini / ESP8266 compile: passed.
  - Command: `arduino-cli compile --fqbn esp8266:esp8266:d1_mini firmware\BedroomLedController`
  - RAM globals/statics: 69,660 / 80,192 bytes (86%).
  - IRAM: 61,383 / 65,536 bytes (93%).
  - Flash/IROM: 639,460 / 1,048,576 bytes (60%).

## D1 mini OTA attempt

- OTA upload command: `arduino-cli upload --fqbn esp8266:esp8266:d1_mini --port 192.168.1.201 --protocol network --input-dir build\d1mini-ota --discovery-timeout 10s --upload-field password=<local secret> firmware\BedroomLedController`
- OTA tool result: authenticated and exited successfully.
- Uploaded binary: `BedroomLedController.ino.bin`, 700,128 bytes.
- Post-upload verification: failed. The D1 mini did not return on `http://192.168.1.201/api/resources`, port 80, OTA port 8266, Arduino network discovery, or visible fallback AP scan.
- Current live status: upload transfer succeeded at the tool layer, but running firmware is not verified after reboot.

## Physical testing

- not physically tested.
