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
public:
    std::array<AudioFrame<T, N>, W> _window;
};

#endif /*AUDIO_BUFFER_H*/