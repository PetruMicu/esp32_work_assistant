#include "AudioInput.hpp"

AudioInput::AudioInput() : _recording(false), _configured(false) {}

AudioInput::~AudioInput()
{
    if (_recording) 
        stopRecording();
    i2s_del_channel(_rx_handle);
}

bool AudioInput::init()
{
    if (true == _configured)
        return false;

    configureI2S();
    return true;
}

void AudioInput::configureI2S()
{
    esp_err_t status = ESP_OK;

    _chan_cfg = 
    {
        .id = I2S_PORT,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = DMA_DESC_NUM,
        .dma_frame_num = DMA_FRAME_NUM,
        .auto_clear = false,
    };
    /* Allocate a new RX channel and get the handle of this channel */
    status = i2s_new_channel(&_chan_cfg, NULL, &_rx_handle);

    if (ESP_OK == status)
    {
        _std_cfg = 
        {
            .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
            .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
            .gpio_cfg = 
            {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = I2S_SCK,
                .ws = I2S_WS,
                .dout = I2S_GPIO_UNUSED,
                .din = I2S_SD,
                .invert_flags = 
                {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false,
                },
            },

        };
        /* Initialize the channel */
        status = i2s_channel_init_std_mode(_rx_handle, &_std_cfg);
        
        if (ESP_OK == status)
        {
            printf("Audio config success\n");
            _configured = true;
        }
        else
        {
            printf("Audio config fail: %ld\n", (uint32_t)status);
        }
    }
    else
    {
        printf("Audio config fail: %ld\n", (uint32_t)status);
    }
}

void AudioInput::startRecording()
{
    esp_err_t status;

    if (false == _recording)
    {
        _recording = true;
        status = i2s_channel_enable(_rx_handle);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (ESP_OK == status)
        {
            printf("Audio enable success\n");
            xTaskCreate(sampleTask,               // Task function
                        "SampleTask",             // Task name (for debugging)
                        4096,                     // Stack size (adjust as needed)
                        this,                     // Task parameter
                        configMAX_PRIORITIES - 1, // Task priority (adjust as needed)
                        &_sample_task_handler);
        }
        else
        {
            printf("Audio enable fail: %ld\n", (uint32_t)status);
        }
    }
}

void AudioInput::stopRecording()
{
    esp_err_t status;
    
    if (true == _recording)
    {
        _recording = false;
        status = i2s_channel_disable(_rx_handle);

        if (ESP_OK == status)
        {
            printf("Audio disable success\n");
            vTaskDelete(_sample_task_handler);
        }
        else
        {
            printf("Audio disable fail: %ld\n", (uint32_t)status);
        }
    }
}

size_t AudioInput::readData(int32_t* samples)
{
    size_t bytesRead;
    esp_err_t status;

    status = i2s_channel_read(_rx_handle, samples, (AUDIO_BUFFER_SIZE * sizeof(int32_t)), &bytesRead, portMAX_DELAY);
    if (ESP_OK != status)
    {
        printf("Audio read fail: %ld\n", (uint32_t)status);
    }
    else if ((AUDIO_BUFFER_SIZE * sizeof(AUDIO_DATA_TYPE)) != bytesRead)
    {
        printf("Audio read fail: Bytes read %d\n", bytesRead);
    }

    return bytesRead;
}

void sampleTask(void* pvParameter) {
    AudioInput* audio_input = (AudioInput*)pvParameter;
    std::array<std::array<int32_t, AUDIO_BUFFER_SIZE>, AUDIO_QUEUE_SIZE> sample_frames;
    size_t read_index = 0;
    
    while (1) {
        (void)audio_input->readData((sample_frames[read_index]).data());
        xQueueSend(audio_queue, (sample_frames[read_index]).data(), portMAX_DELAY);
        read_index = (size_t)((read_index + 1U) % AUDIO_QUEUE_SIZE);

        vTaskDelay(pdMS_TO_TICKS(AUDIO_POLLING_TIME));
    }
}