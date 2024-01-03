#pragma once

#include <stdint.h>
#include <stdio.h>
#include <array>
#include <algorithm>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Macros.hpp"

class AudioFrame {
using FrameShape = std::array<AUDIO_DATA_TYPE, AUDIO_BUFFER_SIZE>;
private:
    std::array<AUDIO_DATA_TYPE, AUDIO_BUFFER_SIZE> _frame;

public:
    AudioFrame();
    AudioFrame(const AUDIO_DATA_TYPE* array);
    AUDIO_DATA_TYPE* accessFrame();
    void writeFrame(const AUDIO_DATA_TYPE* array);
    FrameShape::iterator begin() { return _frame.begin(); }
    FrameShape::iterator end()   { return _frame.end(); }
};

class AudioBuffer {
private:
    std::array<AudioFrame, AUDIO_WINDOW_SIZE> _window;
    std::size_t _write_index;
    std::size_t _read_index;
    std::size_t _frames_in_one_sec;
    std::size_t _frames_in_buffer;
public:
    
    AudioBuffer();
    void pushFrame(const AudioFrame &frame);
    AudioFrame popFrame();
    bool gotOneSecond();
};
