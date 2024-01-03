#include "AudioBuffer.hpp"

AudioFrame::AudioFrame(const AUDIO_DATA_TYPE* array) {
    _write_index = 0u;
    _read_index = 0u;
    _frames_in_one_sec = 0u;
    _frames_in_buffer = 0u;
    writeFrame(array);
}

AudioFrame::AudioFrame(void) {
    _write_index = 0u;
    _read_index = 0u;
    _frames_in_one_sec = 0u;
    _frames_in_buffer = 0u;
    std::fill(_frame.begin(), _frame.end(), AUDIO_DATA_TYPE{0});
}

AUDIO_DATA_TYPE* AudioFrame::accessFrame()
{
    return _frame.data();
}

void AudioFrame::writeFrame(const AUDIO_DATA_TYPE* array) {
    for (std::size_t i = 0; i < AUDIO_BUFFER_SIZE; ++i) {
        _frame[i] = array[i];
    }
}

AudioBuffer::AudioBuffer() : _write_index(0), _read_index(0) 
{
    _frames_in_one_sec = SAMPLE_RATE / AUDIO_BUFFER_SIZE;
}

void AudioBuffer::pushFrame(const AudioFrame &frame)
{
    _window[_write_index] = frame;
    _write_index = (_write_index + 1) % AUDIO_WINDOW_SIZE;

    // Handle case when write index overtakes read index
    if (_write_index == _read_index) {
        _read_index = (_read_index + 1) % AUDIO_WINDOW_SIZE;
    }
    else {
        _frames_in_buffer++;
    }
}

AudioFrame AudioBuffer::popFrame() 
{
    AudioFrame frame = _window[_read_index];
    _read_index = (_read_index + 1) % AUDIO_WINDOW_SIZE;
    _frames_in_buffer--;

    return frame;
}

bool AudioBuffer::gotOneSecond(){
    return (_frames_in_buffer == _frames_in_one_sec);
}