# NOR Flash drivers for ESP-IDF

This component provides some 3rd party NOR flash drivers for ESP-IDF esp_flash driver.

Please be aware of:

- Espressif will do basic reviewing (format, simple logic check, etc.) for the PRs
- Contributors should do functionality tests on actual flash chips
- Users should do all necessary tests, including integrated tests, stress tests, stability tests themselves. Users should be responsible for:
    - Verify 3rd party flash drivers.
    - Verify related flash models (including their stability, continuous supply) before selecting them into the supply chain. Note supply voltage and temperature may affect a lot to the performance of the flash chips.

# Usage example

There is an [simple example](https://github.com/espressif/esp-idf/tree/master/examples/storage/custom_flash_driver) about how to use this component. You can also follow [this instruction](https://github.com/espressif/esp-idf/blob/master/examples/storage/custom_flash_driver/README.md) for how to implement a customized driver for your flash with this component.

Contributions for more customized flash driver, or bug fixes are appreciated.

# Note

1. All flash related code/data should be put into RAM, so please add `IRAM_ATTR` for function and `DRAM_ATTR` for data.

# Error handling

1. **Cache disabled but cached memory region accessed** : Please check whether all function and data have been put into RAM memory.

# Contributions

See https://github.com/espressif/esp-flash-drivers/blob/main/README.md on how to make contributions!

If you find any issue, please report to https://github.com/espressif/esp-flash-drivers/issues.
