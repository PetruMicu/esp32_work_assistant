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
    AUDIO_DATA_TYPE* _ptr_to_fft1;
    AUDIO_DATA_TYPE* _ptr_to_fft2;
    AudioBuffer* _sample_buffer;
    bool _configured;
    void performFFT();
public:
    AudioProcessor();
    bool init(AudioBuffer& samples);
    void deinit();
    void computeSpectogram(AUDIO_DATA_TYPE* spectogram, std::size_t audio_frames);
};