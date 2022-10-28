# NOR Flash drivers for ESP-IDF

This component provides some 3rd party NOR flash drivers for ESP-IDF esp_flash driver.

Please be aware of:

- Espressif will do basic reviewing (format, simple logic check, etc.) for the PRs
- Contributors should do functionality tests on actual flash chips
- Users should do all neessary tests, including integrated tests, stress tests, stability tests themselves. Users should be responsible for:
    - Verify 3rd party flash drivers.
    - Verify related flash models (including their stability, continuous supply) before selecting them into the supply chain. Note supply voltage and temperature may affect a lot to the performance of the flash chips.

# How to implement your driver

## Bootloader flash driver

To implement bootloader driver in order that the esp chip can boot successfully, please read this part carefully. Any wrong code will make esp chip crashes. So, please fully comprehend what to do and how to do before making any changes.

### 1. `bootloader_flash_unlock`

- `Bootloader_flash_unlock` function  is used to unlock flash write protection, which means once the flash is locked by accident, IDF will be able to unlock it.
- The unlock function will erase all bits in Status Register and Configuration Register except QE and some timing registers.
- Ensure that all the commands should be sent out and recognized successfully, otherwise, the flash may keep working while the protection is disabled, which might lead to mis-writing/erasing, mis-locked issues.
- Please don't touch QE bits and timing related bits, otherwise, your chip will not be able to run under QUAD modes, or timing sample will go wrong.

`bootloader_flash_unlock` function is put in `bootloader_flash_unlock_override.c`, we have provide 3 basic behaviors in that file, what you need to do is to check first.

- case 1: Please check whether QE bit in your flash chip is placed at bit 1 in status register-2, and your flash supports using command 01H to write two bytes in status register. If so, congratulations that you need to do nothing, that's our default behavior.
- case 2: Please check whether QE bit in your flash chip is placed at bit 1 in status register-2, and your flash only supports using command 31H to write status register-2 (In other words, your flash doesn't support 01H+2bytes). If so , you need to add your chip ID in function `is_qe_bit_in_bit1_at_reg2`, for example:
```C
FORCE_INLINE_ATTR bool is_qe_bit_in_bit1_at_reg2(const esp_rom_spiflash_chip_t* chip)
{
    bool ret = true;
    switch (chip->device_id) {
    /****GD series***/
    case 0xC84016:
    case 0xC84017:
    case 0xC84018:
        break;
    /**** your flash series ****/
    case /*your flash ID*/:
        break;
    default:
        ret = false;
    }
    return ret;
}
```
- case 3: Please check whether QE bit in your flash chip is placed at bit 6 in status register-1, and your flash chip supports using command 01H to write status register-1. If so, you need to add your chip ID in function `is_qe_bit_in_bit6_at_reg1`, for example:
```C
FORCE_INLINE_ATTR bool is_qe_bit_in_bit6_at_reg1(const esp_rom_spiflash_chip_t* chip)
{
    bool ret = true;
    switch (chip->device_id) {
    /***ISSI series***/
    case 0x9D4016:
    case 0x9D4017:
        break;

    /***MXIC series***/
    case 0xC22016:
    case 0xC22017:
        break;

    /****your clash series***/
    case /*your flash ID*/:
        break;
    default:
        ret = false;
    }
    return ret;
}
```
- case 4: If 3 cases mentioned above cannot cover your usage. Please add a function named `is_qe_bit_in_bit_x_at_reg_x_(appendix_behavior)` and related branch and behavior in function `bootloader_flash_unlock`.

### 2. `bootloader_flash_qe_support_list`

`bootloader_flash_qe_support_list` is used for iteration in bootloader for selecting the correct behavior to enable flash chip work under QUAD mode. In order to make flash works under QUAD mode, enable QE bit in flash status register is necessary. If you want to use your flash chip under QUAD mode, please read this part and make implementation for yourself.

* case 1: If QE bit is placed at bit1 in status register-2, you need to do nothing because that is our default behavior.
* case 2: If QE bit on your chip is placed at different places, or need to use different command. Please add your own support. You can take the function from [flash_qio_mode.c](https://github.com/espressif/esp-idf/blob/master/components/bootloader_support/bootloader_flash/src/flash_qio_mode.c). If functions there cannot fully cover your usage, you can define your own function with `bootloader_execute_flash_command`. Then, you need to place chip name, chip ID, chip behaviors, etc. into `bootloader_flash_qio_support_list`. For example
```C
const bootloader_qio_info_t bootloader_flash_qe_support_list[] = {
    /*   Manufacturer,   mfg_id, flash_id, id mask, Read Status,                Write Status,               QIE Bit */
    { "MXIC",        0xC2,   0x2000, 0xFF00,    bootloader_read_status_8b_rdsr,        bootloader_write_status_8b_wrsr,       6 },
    { "ISSI",        0x9D,   0x4000, 0xCF00,    bootloader_read_status_8b_rdsr,        bootloader_write_status_8b_wrsr,       6 },
    { "WinBond",     0xEF,   0x4000, 0xFF00,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
    { "GD",          0xC8,   0x6000, 0xFF00,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
    { "XM25QU64A",   0x20,   0x3817, 0xFFFF,    bootloader_read_status_8b_xmc25qu64a,  bootloader_write_status_8b_xmc25qu64a, 6 },
    { "TH",          0xCD,   0x6000, 0xFF00,    bootloader_read_status_16b_rdsr_rdsr2, bootloader_write_status_16b_wrsr,      9 },
    { "EON",         0x1C,   0x7000, 0xFF00,    bootloader_read_status_8b_eon,        bootloader_write_status_8b_eon,       6 },

    /* Final entry is default entry, if no other IDs have matched.

       This approach works for chips including:
       GigaDevice (mfg ID 0xC8, flash IDs including 4016),
       FM25Q32 (QOUT mode only, mfg ID 0xA1, flash IDs including 4016)
       BY25Q32 (mfg ID 0x68, flash IDs including 4016)
    */
    { NULL,          0xFF,    0xFFFF, 0xFFFF,   bootloader_read_status_8b_rdsr2,       bootloader_write_status_8b_wrsr2,      1 },
};
```

## App flash driver

To implement APP flash driver in order that your flash chip can be used to read, write, erase, etc. to save data. Or more advanced usage like OTA. So, it's very important in this part, and following content will give you a guide for how to implement your flash driver.

- STEP 1: Please check what behavior is different from our default behavior, including but not limited to different commands, different dummys, different data bytes, different status registers. You can look into [default_chip_behavior](https://github.com/espressif/esp-idf/blob/master/components/spi_flash/spi_flash_chip_generic.c) to get our default chip behavior.
- STEP 2: If you have found something really different from our default behavior, you need to implement your own driver object. Create a new file naming `spi_flash_chip_xx.c` under path `esp_flash_nor/`, implementing specific behavior there. Note: not all behavior need to be rewrote, you only need to implement the different part.
- STEP 3: Point to functions with function pointer in `spi_flash_chip_t`. You can take EON as an example `spi_flash_eon.c`, for example:
```C
const DRAM_ATTR spi_flash_chip_t esp_flash_chip_eon = {
    .name = chip_name,
    .timeout = &spi_flash_chip_generic_timeout,  /*<! default behavior*/>
    .probe = spi_flash_chip_eon_probe,   /*<! EON specific */
    .reset = spi_flash_chip_generic_reset,
    .detect_size = spi_flash_chip_generic_detect_size,
    .erase_chip = spi_flash_chip_generic_erase_chip,
    .erase_sector = spi_flash_chip_generic_erase_sector,
    .erase_block = spi_flash_chip_generic_erase_block,
    .sector_size = 4 * 1024,
    .block_erase_size = 64 * 1024,

    .get_chip_write_protect = spi_flash_chip_generic_get_write_protect,
    .set_chip_write_protect = spi_flash_chip_generic_set_write_protect,

    .num_protectable_regions = 0,
    .protectable_regions = NULL,
    .get_protected_regions = NULL,
    .set_protected_regions = NULL,

    .read = spi_flash_chip_generic_read,
    .write = spi_flash_chip_generic_write,
    .program_page = spi_flash_chip_generic_page_program,
    .page_size = 256,
    .write_encrypted = spi_flash_chip_generic_write_encrypted,

    .wait_idle = spi_flash_chip_generic_wait_idle,
    .set_io_mode = spi_flash_chip_eon_set_io_mode,
    .get_io_mode = spi_flash_chip_eon_get_io_mode,

    .read_reg = spi_flash_chip_generic_read_reg,
    .yield = spi_flash_chip_generic_yield,
    .sus_setup = spi_flash_chip_eon_suspend_cmd_conf,
    .get_chip_caps = spi_flash_chip_eon_get_caps,
};
```
- STEP 4: Add `spi_flash_chip_xx.h` under path `esp_flash_nor/include/`, add new `.c` file to CMakeLists.txt. Add configuration in `Kconfig`. List your chip object under `default_registered_chips` in `chip_drivers.c` wrapped with  `CONFIG_OPTIONS`.

# Example

We use EON as our first example, you can global search key word `EON` and you can find what need to do and how to add an 3rd party flash. If there are something or step not clear enough, please report [issues](https://github.com/espressif/esp-flash-drivers/issues).

# Note

1. All flash related code/data should be put into RAM, so please add `IRAM_ATTR` for function and `DRAM_ATTR` for data.

# Error handling

1. **Cache disabled but cached memory region accessed** : Please check whether all function and data have been put into RAM memory.


# Advanced

1. All the chip selected config is enabled by default, you can disable them if You are sure that you won't use that flash. So, related code will not be linked in to `IRAM`, and you will have more `RAM` memory.

# Contributions

See https://github.com/espressif/esp-flash-drivers/blob/main/README.md on how to make contributions!

If you find any issue, please report to https://github.com/espressif/esp-flash-drivers/issues.
