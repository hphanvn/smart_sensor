set(component_srcs "dht11.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES driver
                       PRIV_REQUIRES esp_rom
                       INCLUDE_DIRS ".")
