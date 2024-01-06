#pragma once

#include <stdint.h>
#include <stdio.h>
#include <array>
#include <algorithm>
#include <cmath>
#include "esp_dsp.h"
#include "AudioBuffer.hpp"

class AudioProcessor {
private:
    std::array<AUDIO_DATA_TYPE, AUDIO_BUFFER_SIZE> _window;
    std::array<AUDIO_DATA_TYPE, 2U * AUDIO_BUFFER_SIZE> _fft;
    std::array<AUDIO_DATA_TYPE, FFT_FREQ_BINS> _avg_fft;
    AudioBuffer* _sample_buffer;
    AUDIO_DATA_TYPE _global_mean;
    AUDIO_DATA_TYPE _global_max;
    bool _no_overlap;
    bool _configured;
    void performFFT();
    void analyzeFrames(std::size_t audio_frames);
public:
    AudioProcessor();
    bool init(AudioBuffer& samples);
    void deinit();
    void computeSpectogram(AUDIO_DATA_TYPE* spectogram, std::size_t audio_frames);
};