idf_component_register(SRCS "src/AudioBuffer.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES driver
                       REQUIRES audio_input)