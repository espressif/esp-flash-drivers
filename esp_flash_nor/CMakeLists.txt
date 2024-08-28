if(BOOTLOADER_BUILD)
    idf_component_register(SRCS "bootloader_flash_driver/bootloader_flash_custom.c"
                           PRIV_REQUIRES bootloader_support spi_flash
                           INCLUDE_DIRS bootloader_flash_driver/include
                           INCLUDE_DIRS "")
else()
    idf_component_register(SRCS
                                "app_flash_driver/esp_flash_eon/spi_flash_chip_eon.c"
                        INCLUDE_DIRS app_flash_driver/include
                        PRIV_REQUIRES spi_flash bootloader_support esp_common
                        INCLUDE_DIRS ""
                        LDFRAGMENTS linker.lf)

    idf_component_add_link_dependency(FROM spi_flash)
endif()
