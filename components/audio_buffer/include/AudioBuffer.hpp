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
    AUDIO_DATA_TYPE _mean;
public:
    AudioFrame();
    AudioFrame(const int32_t* array);
    AUDIO_DATA_TYPE* accessFrame();
    AUDIO_DATA_TYPE readSample(std::size_t offset);
    void writeSample(std::size_t offset, AUDIO_DATA_TYPE sample);
    void writeFrame(const int32_t* array);
    AUDIO_DATA_TYPE getMean();
    FrameShape::iterator begin() { return _frame.begin(); }
    FrameShape::iterator end()   { return _frame.end(); }
    const AUDIO_DATA_TYPE& operator[](size_t index) const {
        if (index >= AUDIO_BUFFER_SIZE) {
            printf("Error: Out of bound accessing\n");
            return _frame[0U];
        }
        return _frame[index];
    }
    AUDIO_DATA_TYPE& operator[](size_t index) {
        if (index >= AUDIO_BUFFER_SIZE) {
            printf("Error: Out of bound accessing\n");
            return _frame[0U];
        }
        return _frame[index];
    }
};

class AudioBuffer {
private:
    std::array<AudioFrame, AUDIO_WINDOW_SIZE> _window;
    std::size_t _overlap_index;
    std::size_t _write_index;
    std::size_t _read_index;
    std::size_t _frames_in_buffer;
public:
    
    AudioBuffer();
    void pushFrame(const AudioFrame &frame);
    AudioFrame popFrame();
    AudioFrame viewFrame(std::size_t offset);
    void resetOverlap();
    AudioFrame overlapFrames(std::size_t hop_size);
    std::size_t getFramesInBuffer() { return _frames_in_buffer; }
    void printOneSecond();
};
