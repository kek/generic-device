idf_component_register(SRCS "micro.c"
                            "st7735.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_system spi_flash)
