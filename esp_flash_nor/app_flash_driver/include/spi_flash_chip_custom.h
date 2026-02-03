#pragma once

#include <stdint.h>
#include "esp_flash.h"
#include "esp_idf_version.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 0)
#include "esp_flash_chips/spi_flash_chip_driver.h"
#else
#include "spi_flash_chip_driver.h"
#endif

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
