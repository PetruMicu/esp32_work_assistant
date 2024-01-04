idf_component_register(SRCS "src/AudioProcessor.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES driver header audio_buffer espressif__esp-dsp)