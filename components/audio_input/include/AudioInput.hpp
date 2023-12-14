#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include <stdint.h>
#include <stdio.h>
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*INMP441 microphone connections*/
#define I2S_WS GPIO_NUM_25
#define I2S_SD GPIO_NUM_33
#define I2S_SCK GPIO_NUM_32

/*I2S configurations*/
#define SAMPLE_RATE 16384U
#define I2S_PORT I2S_NUM_0
#define DMA_DESC_NUM 6U
#define DMA_FRAME_NUM 256U

class AudioInput
{
private:
    bool _recording;
    bool _configured;

    void configureI2S();
public:
    i2s_chan_handle_t _rx_handle;
    i2s_chan_config_t _chan_cfg;
    i2s_std_config_t _std_cfg;

    AudioInput();
    ~AudioInput();

    bool init();
    void startRecording();
    void stopRecording();
    size_t readData(int32_t* buffer, size_t size);
};

#endif /*AUDIO_INPUT_H*/
