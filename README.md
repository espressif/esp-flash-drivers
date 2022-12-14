# esp-flash-drivers

This is the repository for 3rd party flash drivers and maintained by Espressif.

Please be aware of:

- Espressif will do basic reviewing (format, simple logic check, etc.) for the PRs
- Contributors should do functionality tests on actual flash chips
- Users should do all neessary tests, including integrated tests, stress tests, stability tests themselves. Users should be responsible for:
    - Verify 3rd party flash drivers.
    - Verify related flash models (including their stability, continuous supply) before selecting them into the supply chain. Note supply voltage and temperature may affect a lot to the performance of the flash chips.

# Contributions

Your contributions (PR) are welcome!

Please also attach the datasheet or link of the flash model if possible, so that we can review more effectively.

Contributors should do functionality test before creating PRs!

If you find any issue, please report on https://github.com/espressif/esp-flash-drivers/issues.
