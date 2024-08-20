#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief read bootloader status register in eon rule
 */
unsigned bootloader_read_status_otp_mode_8b(void);

/**
 * @brief write bootloader status register in eon rule
 *
 * @param new_status 
 */
void bootloader_write_status_otp_mode_8b(unsigned new_status);

#ifdef __cplusplus
}
#endif
