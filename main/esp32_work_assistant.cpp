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
#include "Client.hpp"

QueueHandle_t audio_queue;
AudioInput microphone;
AudioBuffer samples;
AudioProcessor processor;
TensorFlowModel model;
Wifi wifi_interface;
Client client(HOST_IP_ADDR);


void processTask(void* pvParameter) {
    bool awake = false;
    int32_t count = 0;
    int32_t raw_samples[AUDIO_BUFFER_SIZE];
    AudioFrame sample_frame;

    if (false == processor.init(samples)) {
        printf("Error: Processor Init Failed\n");
        return;
    }
    while (1) {
        if (xQueueReceive(audio_queue, raw_samples, portMAX_DELAY) == pdPASS) {
                sample_frame.writeFrame(raw_samples);
        }
        if (false == awake) {
            samples.pushFrame(sample_frame);
            if (AUDIO_NO_FRAMES_IN_SOUND_DURATION == samples.getFramesInBuffer()) {
                processor.computeSpectrogram(model.getInput(), AUDIO_NO_FRAMES_IN_SOUND_DURATION);
                float result = model.predict();
                printf("Model prediction: %.2f\n", result);
                if (result >= 0.95) {
                    // microphone.stopRecording();
                    samples.clearBuffer();
                    awake = true;
                    printf("ESP32 WorkStation awake and listening\n");
                    printf("3...\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    printf("2...\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    printf("1...\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    printf("GOO!!\n");
                    /*deinit for TensorFLowModel has some problems*/
                    // model.deinit();
                }
            }
        } else {
            if (false == client.connectToHost()) {
                printf("Error: Client connection failed\n");
                printf("ESP32 WorkStation went to sleep\n");
                awake = false;
            }
            if (false == client.sendAudioSamples(raw_samples)) {
                client.disconnectFromHost();
                printf("ESP32 WorkStation went to sleep\n");
                awake = false;
            } else {
                count++;
                if (AUDIO_NO_FRAMES_IN_SOUND_DURATION == count) {
                    printf("Finished sending audio to host\n");
                    count = 0;
                    printf("ESP32 WorkStation went to sleep\n");
                    awake = false;
                    client.disconnectFromHost();
                }
            }
        }
        /*Wait for data from the microphone*/
        vTaskDelay(pdMS_TO_TICKS(AUDIO_POLLING_TIME));
    }
}

extern "C" void app_main() {
    audio_queue = xQueueCreate(AUDIO_QUEUE_SIZE, sizeof(AudioFrame));
    if (false == wifi_interface.init()) {
        printf("Wifi initialization failed\n");
        return;
    }

    if (false == client.init()) {
        printf("Client initialization failed\n");
        return;
    }

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

    xTaskCreate(processTask,
                "ProcessTask",
                16384,
                NULL,
                configMAX_PRIORITIES - 1,
                NULL);
}
