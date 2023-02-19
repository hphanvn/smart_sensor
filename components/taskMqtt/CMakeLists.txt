set(component_srcs "taskMqtt.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES main
                       INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
#Compile as Cpp
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++2b)                       