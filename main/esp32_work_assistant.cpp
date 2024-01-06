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
TaskHandle_t wifi_task_handler;
TaskHandle_t process_task_handler;


void processTask(void* pvParameter) {
    bool status;
    int32_t raw_samples[AUDIO_BUFFER_SIZE];
    AudioFrame sample_frame;

    status = processor.init(samples);
    if (false == status) {
        printf("Error: Processor Init Failed\n");
        return;
    }
    /*using this bool to check if wake word was detected*/
    status = false;
    while (1) {
        if (false == status) {
            if (xQueueReceive(audio_queue, raw_samples, portMAX_DELAY) == pdPASS) {
                sample_frame.writeFrame(raw_samples);
                samples.pushFrame(sample_frame);
                if (AUDIO_NO_FRAMES_IN_SOUND_DURATION == samples.getFramesInBuffer())
                {
                    processor.computeSpectogram(model.getInput(), AUDIO_NO_FRAMES_IN_SOUND_DURATION);
                    float result = model.predict();
                    printf("Model prediction: %.2f\n", result);
                    if (result >= 0.95) {
                        microphone.stopRecording();
                        samples.clearBuffer();
                        status = true;
                        /*deinit for TensorFLowModel has some problems*/
                        // model.deinit();
                        xTaskNotify(*((TaskHandle_t*)pvParameter), 0, eNoAction);
                        /*Notify other task*/
                    }
                }
            }
            /*Wait for data from the microphone*/
            vTaskDelay(pdMS_TO_TICKS(AUDIO_POLLING_TIME));
        } else {
            /*Wait for device to go to sleep*/
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            /*Device went to sleep, start listening for wake word*/
            microphone.startRecording();
            status = false;
        }
    }
}

void WifiTask(void* pvParameter) {
    while(1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        printf("ESP32 WorkStation awake and listening\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
        // send data to server via wifi /*TO DO*/
        /*Put device to sleep*/
        xTaskNotify(*((TaskHandle_t*)pvParameter), 0, eNoAction);
        printf("ESP32 WorkStation went to sleep\n");
    }
}

extern "C" void app_main() {
    audio_queue = xQueueCreate(AUDIO_QUEUE_SIZE, sizeof(AudioFrame));


    xTaskCreate(WifiTask,               // Task function
                "WifiTask",             // Task name (for debugging)
                1024,                     // Stack size (adjust as needed)
                &process_task_handler,                     // Task parameter
                configMAX_PRIORITIES - 1, // Task priority (adjust as needed)
                &wifi_task_handler);

    xTaskCreate(processTask,               // Task function
                "ProcessTask",             // Task name (for debugging)
                8192,                     // Stack size (adjust as needed)
                &wifi_task_handler,       // Task parameter
                configMAX_PRIORITIES - 1, // Task priority (adjust as needed)
                &process_task_handler);
    
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
    // printf("%d", AUDIO_WINDOW_SIZE);
}
