# Configuration (CLION)
## ESP32C6 (not working)
`west build -- -DEXTRA_CONF_FILE="boards/esp32c6_devkitc.conf;credentials.conf" -DDTC_OVERLAY_FILE=boards/esp32c6_devkitc.overlay`

## ESP32S3 (not working)
`west build -- -DEXTRA_CONF_FILE="boards/esp32s3_devkitc.conf;credentials.conf" -DDTC_OVERLAY_FILE=boards/esp32s3_devkitc.overlay`

## ESP32S2
`west build -- -DEXTRA_CONF_FILE="boards/esp32s2_saola.conf;credentials.conf" -DDTC_OVERLAY_FILE=boards/esp32s2_saola.overlay`

