idf_component_register(SRCS "micro.c"
                            "led.c"
                            "system_info.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_system)
