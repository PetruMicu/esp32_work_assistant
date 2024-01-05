#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "Macros.hpp"
#include "AudioBuffer.hpp"
#include "AudioInput.hpp"
#include "AudioProcessor.hpp"

QueueHandle_t audio_queue;
AudioInput microphone;
AudioBuffer samples;
AudioProcessor processor;
/*this would be part of the neural network*/
AUDIO_DATA_TYPE spectogram[SPECTOGRAM_SIZE];

void processTask(void* pvParameter) {
    int32_t raw_samples[AUDIO_BUFFER_SIZE];
    AudioFrame sample_frame;
    processor.init(samples);
    while (1) {
        if (xQueueReceive(audio_queue, raw_samples, portMAX_DELAY) == pdPASS) {
            sample_frame.writeFrame(raw_samples);
            samples.pushFrame(sample_frame);
            if (AUDIO_NO_FRAMES_IN_SOUND_DURATION == samples.getFramesInBuffer())
            {
                printf("GOT_FRAMES\n");
                // microphone.stopRecording();
                processor.computeSpectogram(spectogram, AUDIO_NO_FRAMES_IN_SOUND_DURATION);
                // for (std::size_t i = 0U; i < SAMPLE_RATE; ++i) {
                //     printf("%.0f\n", spectogram[i]);
                // }
                // samples.printOneSecond();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(AUDIO_POLLING_TIME));
    }
}

extern "C" void app_main() {
    audio_queue = xQueueCreate(AUDIO_QUEUE_SIZE, sizeof(AudioFrame));
    xTaskCreate(processTask,               // Task function
                "ProcessTask",             // Task name (for debugging)
                8192,                     // Stack size (adjust as needed)
                NULL,                     // Task parameter
                configMAX_PRIORITIES - 1, // Task priority (adjust as needed)
                NULL);

    
    if (false == microphone.init())
    {
        printf("Microphone initialization failed\n");
        return;
    }
    microphone.startRecording();
}
