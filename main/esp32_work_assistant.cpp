#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "Macros.hpp"
#include "AudioBuffer.hpp"
#include "AudioInput.hpp"

QueueHandle_t audioQueue;
AudioInput microphone;
AudioBuffer sample_buffer;

void processTask(void* pvParameter) {
    int32_t raw_samples[AUDIO_BUFFER_SIZE];
    AudioFrame sample_frame;
    
    while (1) {
        if (xQueueReceive(audioQueue, raw_samples, portMAX_DELAY) == pdPASS) {
            sample_frame.writeFrame(raw_samples);
            sample_buffer.pushFrame(sample_frame);
            if (sample_buffer.gotOneSecond())
            {

            }
            printf("%f\n", (sample_buffer.popFrame().accessFrame()[0U]));
            // for (auto sample : sample_buffer.popFrame())
            // {
            //     printf("%f\n", (sample));
            // }
        }
        vTaskDelay(pdMS_TO_TICKS(AUDIO_POLLING_TIME));
    }
}

extern "C" void app_main() {
    audioQueue = xQueueCreate(AUDIO_QUEUE_SIZE, sizeof(AudioFrame));
    xTaskCreate(processTask,               // Task function
                "ProcessTask",             // Task name (for debugging)
                4096,                     // Stack size (adjust as needed)
                NULL,                     // Task parameter
                configMAX_PRIORITIES - 1, // Task priority (adjust as needed)
                NULL);

    
    if (false == microphone.init())
    {
        printf("Microphone initialization failed\n");
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    microphone.startRecording();
}
