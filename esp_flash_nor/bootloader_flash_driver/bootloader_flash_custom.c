#include "bootloader_flash_override.h"
#include "bootloader_flash_priv.h"
#include "esp_rom_spiflash.h"
#include "esp_attr.h"

/* Array of known flash chips and data to enable Quad I/O mode

   Manufacturer & flash ID can be tested by running "esptool.py
   flash_id"

   If manufacturer ID matches, and flash ID ORed with flash ID mask
   matches, enable_qio_mode() will execute "Read Cmd", test if bit
   number "QIE Bit" is set, and if not set it will call "Write Cmd"
   with this bit set.

   Searching of this table stops when the first match is found.
 */

IRAM_ATTR unsigned bootloader_read_status_otp_mode_8b(void)
{
    bootloader_execute_flash_command(CMD_OTPEN, 0, 0, 0);  /* Enter OTP mode */
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    uint32_t read_status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);
    bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0);   /* Exit OTP mode */
    return read_status;
}

IRAM_ATTR void bootloader_write_status_otp_mode_8b(unsigned new_status)
{
    bootloader_execute_flash_command(CMD_OTPEN, 0, 0, 0);  /* Enter OTP mode */
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    bootloader_execute_flash_command(CMD_WRSR, new_status, 8, 0);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0);   /* Exit OTP mode */
}
