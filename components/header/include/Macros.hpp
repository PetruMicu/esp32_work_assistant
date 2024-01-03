#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2s_std.h"

typedef int32_t AUDIO_DATA_TYPE;
extern QueueHandle_t audioQueue;

/*INMP441 microphone connections*/
#define I2S_WS GPIO_NUM_25
#define I2S_SD GPIO_NUM_33
#define I2S_SCK GPIO_NUM_32

/*I2S configurations*/
#define SAMPLE_RATE 16384U
#define I2S_PORT I2S_NUM_0
#define DMA_DESC_NUM 6U
#define DMA_FRAME_NUM 256U

/*Sound sampling parameters*/
#define AUDIO_BUFFER_SIZE 128U /*number of samples*/
#define AUDIO_POLLING_TIME ((AUDIO_BUFFER_SIZE / (SAMPLE_RATE / 1000U)) * 1.1) /*time needed to gather enough samples*/
#define AUDIO_SOUND_DURATION 1U /*duration in seconds*/
#define AUDIO_WINDOW_SIZE (SAMPLE_RATE / AUDIO_BUFFER_SIZE + 5U)
#define AUDIO_QUEUE_SIZE 3U
