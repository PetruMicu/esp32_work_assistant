#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include <stdint.h>
#include <stdio.h>
#include <array>
#include "AudioInput.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*T is data width and N is frame length*/
template <typename T, std::size_t N>
class AudioFrame {
private:
    std::array<T, N> _frame;

public:

};

/*T is data width and N is frame length*/
template <typename T, std::size_t N, std::size_t W>
class AudioBuffer {
private:
    std::array<AudioFrame<T, N>, W> _window;
    std::size_t _writeIndex;
    std::size_t _readIndex;
    std::size_t _frames_in_one_sec;
public:
    
    AudioBuffer(std::size_t sample_rate);
    void pushFrame(const AudioFrame<T, N> &frame);
    AudioFrame<T, N> popFrame();
    
};

#endif /*AUDIO_BUFFER_H*/