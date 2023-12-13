#include "AudioInput.hpp"

AudioInput::AudioInput() : recording(false), configured(false) {}

AudioInput::~AudioInput()
{
    if (this->recording) 
        stopRecording();
    i2s_driver_uninstall(i2sPort);
}

bool AudioInput::init()
{
    if (true == this->configured)
        return false;

    this->configureI2S();
    return true;
}

void AudioInput::configureI2S()
{
    i2sPort = I2S_NUM_0;

    i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // Receive audio
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    
    pinConfig = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };

    i2s_driver_install(i2sPort, &i2sConfig, 0, NULL);
    i2s_set_pin(i2sPort, &pinConfig);
    // i2s_stop(i2sPort);
    this->configured = true;
}

void AudioInput::startRecording()
{
    if (false == this->recording)
    {
        recording = true;
        i2s_start(i2sPort);
    }
}

void AudioInput::stopRecording()
{
    if (true == this->recording)
    {
        recording = false;
        i2s_stop(i2sPort);
    }
}

size_t AudioInput::readData(int32_t* buffer, size_t size)
{
    size_t bytesRead;

    (void)i2s_read(i2sPort, buffer, size, &bytesRead, portMAX_DELAY);

    return bytesRead;
}