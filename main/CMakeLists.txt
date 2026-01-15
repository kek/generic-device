idf_component_register(SRCS "micro.c"
                            "st7735.c"
                            "space_invaders.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_system spi_flash)
