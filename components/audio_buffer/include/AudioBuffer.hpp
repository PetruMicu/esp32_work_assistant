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
    AUDIO_DATA_TYPE _max;
    AUDIO_DATA_TYPE _mean;
public:
    AudioFrame();
    AudioFrame(const int32_t* array);
    AUDIO_DATA_TYPE* accessFrame();
    void writeFrame(const int32_t* array);
    AUDIO_DATA_TYPE getMax();
    AUDIO_DATA_TYPE getMean();
    FrameShape::iterator begin() { return _frame.begin(); }
    FrameShape::iterator end()   { return _frame.end(); }
    const AUDIO_DATA_TYPE& operator[](size_t index) const;
    AUDIO_DATA_TYPE& operator[](size_t index);
};

class AudioBuffer {
private:
    std::array<AudioFrame, AUDIO_WINDOW_SIZE> _window;
    std::size_t _write_index;
    std::size_t _read_index;
    std::size_t _frames_in_buffer;
public:
    
    AudioBuffer();
    void pushFrame(const AudioFrame &frame);
    AudioFrame popFrame();
    std::size_t getFramesInBuffer() { return _frames_in_buffer; }
    void printOneSecond();
};
