#include "AudioBuffer.hpp"

AudioBuffer::AudioBuffer(std::size_t sample_rate) : _writeIndex(0), _readIndex(0) 
{
    _frames_in_one_sec = _sample_rate / N;
}

void AudioBuffer::pushFrame(const AudioFrame<T, N> &frame)
{
     _window[_writeIndex] = frame;
    _writeIndex = (_writeIndex + 1) % W;

    // Handle case when write index overtakes read index
    if (_writeIndex == _readIndex) {
        _readIndex = (_readIndex + 1) % W;
    }
}

AudioFrame<T, N> AudioBuffer::popFrame() 
{
    AudioFrame<T, N> frame = _buffer[_readIndex];
    _readIndex = (_readIndex + 1) % W;

    return frame;
}



