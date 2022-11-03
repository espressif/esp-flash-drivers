if(BOOTLOADER_BUILD)
    idf_component_register(SRCS "bootloader_flash_driver/bootlaoder_flash_qio_custom.c"
                                "bootloader_flash_driver/bootloader_flash_unlock_override.c"
                           PRIV_REQUIRES bootloader_support spi_flash
                           INCLUDE_DIRS bootloader_flash_driver/include
                           INCLUDE_DIRS "")
    # Forces the linker to include bootloader_unlock from this component
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u bootloader_flash_unlock_override")
else()
    idf_component_register(SRCS
                                "app_flash_driver/spi_flash_chip_eon.c"
                                "bootloader_flash_driver/bootloader_flash_unlock_override.c"
                        INCLUDE_DIRS app_flash_driver/include
                        PRIV_REQUIRES spi_flash bootloader_support esp_common
                        INCLUDE_DIRS "")

    idf_component_get_property(spi_flash_lib spi_flash COMPONENT_LIB)
    set_property(TARGET ${spi_flash_lib} APPEND PROPERTY INTERFACE_LINK_LIBRARIES $<LINK_ONLY:${COMPONENT_LIB}>)
endif()
