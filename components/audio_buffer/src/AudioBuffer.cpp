#include "AudioBuffer.hpp"

AudioFrame::AudioFrame(const int32_t* array) {
    writeFrame(array);
}

AudioFrame::AudioFrame(void) {
    _max = 0U;
    _mean = 0U;
    std::fill(_frame.begin(), _frame.end(), AUDIO_DATA_TYPE{0});
}

AUDIO_DATA_TYPE* AudioFrame::accessFrame()
{
    return _frame.data();
}

void AudioFrame::writeFrame(const int32_t* array) {
    _max = 0U;
    _mean = 0U;
    /*Store samples and find mean value*/
    for (std::size_t i = 0u; i < AUDIO_BUFFER_SIZE; ++i) {
        /*24bit data (0x010203) is stored like this: 0x01 0x02 0x03 0x00*/
        /*8 bit shift to get the data + 3bit shift clean the sample of small noise*/
        _frame[i] = (AUDIO_DATA_TYPE)(array[i] >> 11U);
        _mean += _frame[i];
        if (abs(_frame[i]) > _max) {
            _max = abs(_frame[i]);
        }
    }
    _mean /= AUDIO_BUFFER_SIZE;

    /*Remove mean from the samples and find max value*/
    for (std::size_t i = 0u; i < AUDIO_BUFFER_SIZE; ++i) {
        /*24bit data (0x010203) is stored like this: 0x01 0x02 0x03 0x00*/
        /*8 bit shift to get the data + 3bit shift clean the sample of small noise*/
        _frame[i] -= _mean;
        if (abs(_frame[i]) > _max) {
            _max = abs(_frame[i]);
        }
    }
}

AUDIO_DATA_TYPE AudioFrame::getMax() {
    return _max;
}

AUDIO_DATA_TYPE AudioFrame::getMean() {
    return _max;
}

const AUDIO_DATA_TYPE& AudioFrame::operator[](size_t index) const {
    if (index >= AUDIO_BUFFER_SIZE) {
        printf("Error: Out of bound accessing\n");
        return _frame[0U];
    }
    return _frame[index];
}

AUDIO_DATA_TYPE& AudioFrame::operator[](size_t index) {
    if (index >= AUDIO_BUFFER_SIZE) {
        printf("Error: Out of bound accessing\n");
        return _frame[0U];
    }
    return _frame[index];
}

/*----------------------------------------------------------------------------------------------*/

AudioBuffer::AudioBuffer() : _write_index(0), _read_index(0) 
{
    _write_index = 0u;
    _read_index = 0u;
    _frames_in_buffer = 0u;
}

void AudioBuffer::pushFrame(const AudioFrame &frame)
{
    _window[_write_index] = frame;
    _write_index = (_write_index + 1) % AUDIO_WINDOW_SIZE;

    // Handle case when write index overtakes read index
    if (_write_index == _read_index) {
        _read_index = (_read_index + 1) % AUDIO_WINDOW_SIZE;
        printf("FRAME LOST\n");
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

void AudioBuffer::printOneSecond() {
    printf("STARTED PRINTING SAMPLES\n");
    std::size_t no_frames_in_one_sec = AUDIO_NO_FRAMES_IN_SOUND_DURATION;
    while (no_frames_in_one_sec > 0U)
    {
        AudioFrame frame = popFrame();
        for (auto sample : frame) {
            printf("%f\n", sample);
        }
        no_frames_in_one_sec--;
    }
}
