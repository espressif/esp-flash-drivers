#include "bootloader_flash_priv.h"
#include "esp_rom_spiflash.h"
#include "esp_rom_sys.h"
#include "esp_attr.h"

#define BYTESHIFT(VAR, IDX)    (((VAR) >> ((IDX) * 8)) & 0xFF)
#define ISSI_ID                0x9D
#define MXIC_ID                0xC2
#define GD_Q_ID_HIGH           0xC8
#define GD_Q_ID_MID            0x40
#define GD_Q_ID_LOW            0x16

#define ESP_BOOTLOADER_SPIFLASH_QE_SR1_BIT6            (BIT6)
#define ESP_BOOTLOADER_SPIFLASH_QE_SR2_BIT1            (BIT1)   // QE position when you write 8 bits(for SR2) at one time.
#define ESP_BOOTLOADER_SPIFLASH_QE_SR1_2BYTE_BIT9      (BIT9)   // QE position when you write 16 bits at one time.

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
    default:
        ret = false;
    }
    return ret;
}

FORCE_INLINE_ATTR bool is_qe_bit_in_bit1_at_reg2(const esp_rom_spiflash_chip_t* chip)
{
    bool ret = true;
    switch (chip->device_id) {
    /****GD series***/
    case 0xC84016:
    case 0xC84017:
    case 0xC84018:
        break;
    default:
        ret = false;
    }
    return ret;
}

esp_err_t IRAM_ATTR bootloader_flash_unlock(void)
{
    // At the beginning status == new_status == status_sr2 == new_status_sr2 == 0.
    // If the register doesn't need to be updated, keep them the same (0), so that no command will be actually sent.
    uint16_t status = 0;    // status for SR1 or SR1+SR2 if writing SR with 01H + 2Bytes.
    uint16_t new_status = 0;
    uint8_t status_sr2 = 0;    // status_sr2 for SR2.
    uint8_t new_status_sr2 = 0;
    uint8_t sr1_bit_num = 0;
    esp_err_t err = ESP_OK;
    esp_rom_printf("calls new unlock");
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    if (is_qe_bit_in_bit6_at_reg1(&g_rom_flashchip)) {
        // Currently ISSI & MXIC share the same command and register layout, which is different from the default model.
        // If any code here needs to be modified, check both chips.
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);

        /* Clear all bits in the mask.
        (This is different from ROM esp_rom_spiflash_unlock, which keeps all bits as-is.)
        */
        sr1_bit_num = 8;
        new_status = status & ESP_BOOTLOADER_SPIFLASH_QE_SR1_BIT6;
    } else if (is_qe_bit_in_bit1_at_reg2(&g_rom_flashchip)) {
        /* The GD chips behaviour is to clear all bits in SR1 and clear bits in SR2 except QE bit.
           Use 01H to write SR1 and 31H to write SR2.
        */
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);
        sr1_bit_num = 8;
        new_status = 0;

        status_sr2 = bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8);
        new_status_sr2 = status_sr2 & ESP_BOOTLOADER_SPIFLASH_QE_SR2_BIT1;
    } else {
        /* For common behaviour, like XMC chips, Use 01H+2Bytes to write both SR1 and SR2*/
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8) | (bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8) << 8);

        /* Clear all bits except QE, if it is set.
        (This is different from ROM esp_rom_spiflash_unlock, which keeps all bits as-is.)
        */
        sr1_bit_num = 16;
        new_status = status & ESP_BOOTLOADER_SPIFLASH_QE_SR1_2BYTE_BIT9;
    }

    // When SR is written, set to true to indicate that WRDI need to be sent to ensure the protection is ON before return.
    bool status_written = false;
    // Skip if nothing needs to be changed. Meaningless writing to SR increases the risk during write and wastes time.
    if (status != new_status) {
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WREN, 0, 0, 0);
        bootloader_execute_flash_command(CMD_WRSR, new_status, sr1_bit_num, 0);
        status_written = true;
    }

    if (status_sr2 != new_status_sr2) {
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WREN, 0, 0, 0);
        bootloader_execute_flash_command(CMD_WRSR2, new_status_sr2, 8, 0);
        status_written = true;
    }

    if (status_written) {
        //Call esp_rom_spiflash_wait_idle to make sure previous WRSR is completed.
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0);
    }

    return err;
}

void bootloader_flash_unlock_override(void)
{
}
