# NOR Flash drivers for ESP-IDF

This component provides some 3rd party NOR flash drivers for ESP-IDF esp_flash driver.

Please be aware of:

- Espressif will do basic reviewing (format, simple logic check, etc.) for the PRs
- Contributors should do functionality tests on actual flash chips
- Users should do all neessary tests, including integrated tests, stress tests, stability tests themselves. Users should be responsible for:
    - Verify 3rd party flash drivers.
    - Verify related flash models (including their stability, continuous supply) before selecting them into the supply chain. Note supply voltage and temperature may affect a lot to the performance of the flash chips.

# Contributions

See https://github.com/espressif/esp-flash-drivers/blob/main/README.md on how to make contributions!

If you find any issue, please report to https://github.com/espressif/esp-flash-drivers/issues.
