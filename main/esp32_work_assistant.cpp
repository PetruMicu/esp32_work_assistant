#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "rtc_wdt.h"
#include "AudioInput.hpp"

#define AUDIO_BUFFER_SIZE 512 /*number of samples*/
#define AUDIO_POLLING_TIME ((AUDIO_BUFFER_SIZE / (SAMPLE_RATE / 1000)) * 1.1) /*time needed to gather enough samples*/
#define AUDIO_SOUND_DURATION 5 /*duration in seconds*/

int32_t audioBuffer[AUDIO_BUFFER_SIZE];

void taskFunction(void* pvParameter) {
    AudioInput microphone;
    size_t samples_read = 0;
    size_t bytes_read = 0;
    size_t total_samples_to_read = (size_t)(SAMPLE_RATE * AUDIO_SOUND_DURATION);

    if (false == microphone.init())
    {
        printf("Microphone initialization failed\n");
        return;
    }
    
    microphone.startRecording();
    while (1) {
        if (0 == total_samples_to_read)
        {
            printf("Finished sampling audio\n");
            total_samples_to_read = (size_t)(SAMPLE_RATE * AUDIO_SOUND_DURATION);
        }
        else
        {
            if (total_samples_to_read >= AUDIO_BUFFER_SIZE)
            {
                bytes_read = microphone.readData(audioBuffer, sizeof(int32_t) * AUDIO_BUFFER_SIZE);
            }
            else
            {
                bytes_read = microphone.readData(audioBuffer, sizeof(int32_t) * total_samples_to_read);
            }
            samples_read = bytes_read / sizeof(int32_t);
            total_samples_to_read -= samples_read;
            for (uint16_t idx = 0U; idx < samples_read; idx++)
            {
                /*24bit data (0x010203) is stored like this: 0x01 0x02 0x03 0x00*/
                /*8 bit shift to get the data + 3bit shift clean the sample of small noise*/
                printf("%ld\n", (audioBuffer[idx] >> 11U));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(AUDIO_POLLING_TIME));
    }
}

extern "C" void app_main() {
     xTaskCreate(taskFunction,            // Task function
                "RecordSoundTask",       // Task name (for debugging)
                4096,                     // Stack size (adjust as needed)
                NULL,                     // Task parameter
                configMAX_PRIORITIES - 1, // Task priority (adjust as needed)
                NULL);
}
