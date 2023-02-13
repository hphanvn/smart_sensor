set(component_srcs "taskOledDisplay.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES ssd1306
                       PRIV_REQUIRES driver
                       PRIV_REQUIRES main
                       INCLUDE_DIRS "."
                       )

#Compile as Cpp
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++2b)                       