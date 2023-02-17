set(component_srcs "taskServer.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES esp_netif
                       PRIV_REQUIRES esp_http_server
                       PRIV_REQUIRES driver
                       PRIV_REQUIRES lwip
                       INCLUDE_DIRS "."
                       )

#Compile as Cpp
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++2b)                       