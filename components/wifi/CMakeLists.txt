set(component_srcs "wifi.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES esp_wifi
                       PRIV_REQUIRES protocol_examples_common
                       INCLUDE_DIRS "."
                       )

#Compile as Cpp
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++2b)                       