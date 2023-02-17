set(component_srcs "blufi.c" "blufi_security.c" "blufi_init.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES nvs_flash
                       PRIV_REQUIRES esp_wifi
                       PRIV_REQUIRES bt
                       PRIV_REQUIRES mbedtls
                       INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
