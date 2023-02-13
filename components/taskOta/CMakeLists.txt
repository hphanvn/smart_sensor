set(component_srcs "taskOta.c")

idf_component_register(SRCS "${component_srcs}"
                       PRIV_REQUIRES app_update
                       PRIV_REQUIRES esp_http_client
                       PRIV_REQUIRES esp_https_ota
                       INCLUDE_DIRS "."
                       # Embed the server root certificate into the final binary
                       EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem
                       )

#Compile as Cpp
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++2b)                       