#pragma once

#include <stdint.h>
#include "esp_flash.h"
#include "spi_flash_chip_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * EON SPI flash chip_drv, uses all the above functions for its operations. In
 * default auto detection, this is used as a catchall if a more specific chip_drv
 * is not found.
 */
extern const spi_flash_chip_t esp_flash_chip_eon;

#ifdef __cplusplus
}
#endif
