set(component_srcs "taskDHT11.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES dht11
                       PRIV_REQUIRES main
                       INCLUDE_DIRS ".")
