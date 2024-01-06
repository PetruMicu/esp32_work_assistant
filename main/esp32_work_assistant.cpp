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
#include "TensorFlowModel.hpp"

QueueHandle_t audio_queue;
AudioInput microphone;
AudioBuffer samples;
AudioProcessor processor;
TensorFlowModel model;


void processTask(void* pvParameter) {
    bool status;
    int32_t raw_samples[AUDIO_BUFFER_SIZE];
    AudioFrame sample_frame;

    status = processor.init(samples);
    if (false == status) {
        printf("Error: Processor Init Failed\n");
        return;
    }
    while (1) {
        if (xQueueReceive(audio_queue, raw_samples, portMAX_DELAY) == pdPASS) {
            sample_frame.writeFrame(raw_samples);
            samples.pushFrame(sample_frame);
            if (AUDIO_NO_FRAMES_IN_SOUND_DURATION == samples.getFramesInBuffer())
            {
                // microphone.stopRecording();
                processor.computeSpectogram(model.getInput(), AUDIO_NO_FRAMES_IN_SOUND_DURATION);
                float result = model.predict();
                printf("%.2f\n", result);
                if (result >= 0.9) {
                    printf("WORD FOUND!\n");
                }
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

    if (false == model.init()) {
        printf("Error: Model Init Failed\n");
        return;
    }
    microphone.startRecording();
    printf("%d", AUDIO_WINDOW_SIZE);
}
