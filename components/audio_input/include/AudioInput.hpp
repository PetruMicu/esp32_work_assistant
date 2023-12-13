#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include <stdint.h>
#include <stdio.h>
#include "driver/i2s.h"

#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32

#define SAMPLE_RATE 16000

class AudioInput
{
private:
    bool recording;
    bool configured;

    void configureI2S();
public:
    i2s_config_t i2sConfig;
    i2s_pin_config_t pinConfig;
    i2s_port_t i2sPort;
    AudioInput();
    ~AudioInput();

    bool init();
    void startRecording();
    void stopRecording();
    size_t readData(int32_t* buffer, size_t size);
};

#endif /*AUDIO_INPUT_H*/
