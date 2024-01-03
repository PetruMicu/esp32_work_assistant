#pragma once

#include <stdint.h>
#include <stdio.h>
#include <array>
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Macros.hpp"
#include "AudioBuffer.hpp"

void sampleTask(void* pvParameter);

class AudioInput
{
private:
    bool _recording;
    bool _configured;
    // I2S reader task
    TaskHandle_t _sample_task_handler;

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
    size_t readData(int32_t* audio_frame);
};

