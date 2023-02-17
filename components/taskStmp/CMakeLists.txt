set(component_srcs "taskStmp.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES esp_event
                       PRIV_REQUIRES mbedtls
                       PRIV_REQUIRES esp_https_ota
                       INCLUDE_DIRS "."
                       # Embed the server root certificate into the final binary
                       EMBED_TXTFILES ${project_dir}/server_certs/stmp_cert.pem esp_logo.png
                       )

#Compile as Cpp
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++2b)                       