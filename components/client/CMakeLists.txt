idf_component_register(SRCS "src/Client.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES driver header esp_wifi nvs_flash audio_buffer)