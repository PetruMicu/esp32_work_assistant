#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2s_std.h"

typedef float AUDIO_DATA_TYPE;
extern QueueHandle_t audio_queue;

/*INMP441 microphone connections*/
#define I2S_WS GPIO_NUM_25
#define I2S_SD GPIO_NUM_33
#define I2S_SCK GPIO_NUM_32

/*I2S configurations*/
#define SAMPLE_RATE 16000U
#define I2S_PORT I2S_NUM_0
#define DMA_DESC_NUM 6U
#define DMA_FRAME_NUM 256U

/*Sound sampling parameters*/
#define AUDIO_BUFFER_SIZE 256U /*number of samples*/
#define AUDIO_OVERLAP_FRACTION 0.5 /*overlap fraction of the audio frames for STFT*/
#define AUDIO_HOP_SIZE (1 - AUDIO_OVERLAP_FRACTION) * AUDIO_BUFFER_SIZE  /*hop size for overlaping over audio frames used for STFT*/
#define AUDIO_POLLING_TIME ((AUDIO_BUFFER_SIZE / (SAMPLE_RATE / 1000U)) * 1.1) /*time needed to gather enough samples*/
#define AUDIO_SOUND_DURATION 1U /*duration in seconds*/
#define AUDIO_NO_FRAMES_IN_SOUND_DURATION (int)(ceilf(((float)SAMPLE_RATE / (float)AUDIO_BUFFER_SIZE) * AUDIO_SOUND_DURATION))
#define AUDIO_WINDOW_SIZE (AUDIO_NO_FRAMES_IN_SOUND_DURATION + 5U)
#define AUDIO_QUEUE_SIZE 3U

#define FFT_FRAMES 124 /*(SAMPLE_RATE - AUDIO_BUFFER_SIZE) / AUDIO_HOP_SIZE + 1*/
#define FFT_FREQ_BINS 22 /*AUDIO_BUFFER_SIZE / 2 + 1*/
#define FFT_AVG_POOL_SIZE 6
#define SPECTROGRAM_STEP (int)ceilf(((float)AUDIO_BUFFER_SIZE / 2 + 1) / (float)FFT_AVG_POOL_SIZE)
#define EPSILON 1e-6

#define TENSOR_ARENA_SIZE 20000

#define WIFI_SSID      "Mcu"
#define WIFI_PASS      "Mcu1234*;"
#define HOST_IP_ADDR   "192.168.1.134"
#define PORT           5005
