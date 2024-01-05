#include "AudioProcessor.hpp"

std::size_t dummy_count = 0;

AudioProcessor::AudioProcessor() {
    std::fill(_window.begin(), _window.end(), AUDIO_DATA_TYPE{0});
    std::fill(_fft.begin(), _fft.end(), AUDIO_DATA_TYPE{0});
    _sample_buffer = nullptr;
    _configured = false;
    _no_overlap = true;
    _global_mean = 0U;
    _global_max = 0U;
}

bool AudioProcessor::init(AudioBuffer& samples) {
    esp_err_t status;

    /*Store address of the samples so it can be used to extract data later*/
    _sample_buffer = &samples;

    /*Initialize FFT library*/
    status =  dsps_fft2r_init_fc32(NULL, AUDIO_BUFFER_SIZE);
    if (ESP_OK  != status) {
        printf("Not possible to initialize FFT. Error = %d", status);
        return false;
    }

    // /*Generates Blackman window*/
    // dsps_wind_blackman_f32((float*)_window.data(), AUDIO_BUFFER_SIZE);
    /*Generates Hann window*/
    dsps_wind_hann_f32((float*)_window.data(), AUDIO_BUFFER_SIZE);
    
    _configured = true;

    return true;
}

void AudioProcessor::deinit() {
    std::fill(_window.begin(), _window.end(), AUDIO_DATA_TYPE{0});
    std::fill(_fft.begin(), _fft.end(), AUDIO_DATA_TYPE{0});
    _sample_buffer = nullptr;

    /*Deinitialize FFT library*/
    dsps_fft2r_deinit_fc32();

    _configured = false;
}

void AudioProcessor::performFFT() {
    AudioFrame frame;
    float fft_max = 0U;
    float frequency_step = (SAMPLE_RATE / AUDIO_BUFFER_SIZE);
    std::size_t max_index = 0;

    if (true == _no_overlap)
    {
        _no_overlap = false;
        frame = _sample_buffer->viewFrame(0U);
    } else {
        frame = _sample_buffer->overlapFrames(AUDIO_HOP_SIZE);
    }
    /*Apply window*/
    for (std::size_t i = 0U; i < AUDIO_BUFFER_SIZE; ++i) {
        _fft[i * 2U + 0U] = ((frame[i] - _global_mean) / _global_max) * _window[i];
        /*No imaginary part*/
        _fft[i * 2U + 1U] = 0U;
    }

    dsps_fft2r_fc32(_fft.data(), AUDIO_BUFFER_SIZE);
    dsps_bit_rev_fc32(_fft.data(), AUDIO_BUFFER_SIZE);
    dsps_cplx2reC_fc32(_fft.data(), AUDIO_BUFFER_SIZE);

    for (int i = 0 ; i < AUDIO_BUFFER_SIZE / 2 ; i++) {
        _fft[i] = 10 * log10((_fft[i * 2 + 0] * _fft[i * 2 + 0] + _fft[i * 2 + 1] * _fft[i * 2 + 1]) / AUDIO_BUFFER_SIZE);
        if (_fft[i] > fft_max) {
            fft_max = _fft[i];
            max_index = i;
        }
    }
    
    dummy_count++;
    if (dummy_count == 100)
    {
        printf("Frequency detected: %.2f\n", (float)(max_index * frequency_step));
        dsps_view(_fft.data(), FFT_FREQ_BINS, FFT_FREQ_BINS, 15,  -60, 40, '|');
        dummy_count = 0;
    }
}

void AudioProcessor::computeSpectogram(AUDIO_DATA_TYPE* spectogram, std::size_t audio_frames) {
    if (true == _configured) {
        AUDIO_DATA_TYPE* ptr_to_spectogram_segment = spectogram;
        std::size_t number_of_iterations = FFT_FRAMES;
        /*Process samples by removing mean value and normalize by max value*/
        analyzeFrames(audio_frames);

        for (std::size_t i = 0U; i < number_of_iterations; ++i) {
            if ((i == 0) || (i == (number_of_iterations - 1U))) {
                _no_overlap = true;
            }
            performFFT();
            for (std::size_t j = 0U; j < AUDIO_BUFFER_SIZE/2U + 1; ++j) {
                ptr_to_spectogram_segment[j] = _fft[j];
            }
            ptr_to_spectogram_segment += AUDIO_BUFFER_SIZE/2U + 1;
        }
        /*remove last frame*/
        _sample_buffer->popFrame();
        _sample_buffer->resetOverlap();
        /*reset flag for next processing*/
        _no_overlap = true;
    }
}

void AudioProcessor::analyzeFrames(std::size_t audio_frames) {
    _global_max = 0U;
    _global_mean = 0U;
    for (std::size_t i = 0U; i < audio_frames; ++i) {
        _global_mean += _sample_buffer->viewFrame(i).getMean();
    }

    _global_mean /= audio_frames;

    for (std::size_t i = 0U; i < audio_frames; ++i) {
        AudioFrame frame = _sample_buffer->viewFrame(i);
        for (auto sample : frame) {
            float processed_sample = sample - _global_mean;
            if (processed_sample > _global_max) {
                _global_max = processed_sample;
            }
        }
    }
}
