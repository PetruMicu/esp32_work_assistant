#include "AudioBuffer.hpp"

AudioFrame::AudioFrame(const int32_t* array) {
    writeFrame(array);
}

AudioFrame::AudioFrame(void) {
    _mean = 0U;
    std::fill(_frame.begin(), _frame.end(), AUDIO_DATA_TYPE{0});
}

AUDIO_DATA_TYPE* AudioFrame::accessFrame()
{
    return _frame.data();
}

AUDIO_DATA_TYPE AudioFrame::readSample(std::size_t offset) {
    if (offset >= AUDIO_BUFFER_SIZE) {
        printf("Error: [readSample] Out of bound accessing");
        return 0U;
    }
    return _frame[offset];
}

void AudioFrame::writeSample(std::size_t offset, AUDIO_DATA_TYPE sample) {
    if (offset >= AUDIO_BUFFER_SIZE) {
        printf("Error: [writeSample] Out of bound accessing");
        return;
    }
    _frame[offset] = sample;
}

void AudioFrame::writeFrame(const int32_t* array) {
    _mean = 0U;
    /*Store samples and find mean value*/
    for (std::size_t i = 0u; i < AUDIO_BUFFER_SIZE; ++i) {
        /*24bit data (0x010203) is stored like this: 0x01 0x02 0x03 0x00*/
        /*8 bit shift to get the data + 3bit shift clean the sample of small noise*/
        _frame[i] = (AUDIO_DATA_TYPE)(array[i] >> 11U);
        _mean += _frame[i];
    }
    _mean /= AUDIO_BUFFER_SIZE;
}

AUDIO_DATA_TYPE AudioFrame::getMean() {
    return _mean;
}

/*----------------------------------------------------------------------------------------------*/

AudioBuffer::AudioBuffer() : _write_index(0), _read_index(0) {
    _write_index = 0u;
    _read_index = 0u;
    _frames_in_buffer = 0u;
    _overlap_index = 0U;
}

void AudioBuffer::pushFrame(const AudioFrame &frame) {
    _window[_write_index] = frame;
    _write_index = (_write_index + 1) % AUDIO_WINDOW_SIZE;

    // Handle case when write index overtakes read index
    if (_write_index == _read_index) {
        _read_index = (_read_index + 1) % AUDIO_WINDOW_SIZE;
        printf("Error: Frame Lost\n");
    }
    else {
        _frames_in_buffer++;
    }
}

/*returns a copy of the current frame and removes it*/
AudioFrame AudioBuffer::popFrame() {
    AudioFrame frame = _window[_read_index];
    _read_index = (_read_index + 1) % AUDIO_WINDOW_SIZE;
    _frames_in_buffer--;

    return frame;
}

/*returns a copy of the current frame + offset but does not remove it*/
AudioFrame AudioBuffer::viewFrame(std::size_t offset) {
    return _window[(_read_index + offset) % AUDIO_WINDOW_SIZE];
}

AudioFrame AudioBuffer::overlapFrames(std::size_t hop_size) {
    AudioFrame frame;
    std::size_t sample_idx = 0U;

    if (hop_size > AUDIO_BUFFER_SIZE || hop_size <= 0) {
        printf("Error: Incorrect hop size\n");
        return AudioFrame();
    }

    _overlap_index = (_overlap_index + hop_size) % AUDIO_BUFFER_SIZE;
    if (0U == _overlap_index) {
        return _window[_read_index];
    } else {
        for (std::size_t i = _overlap_index; i < AUDIO_BUFFER_SIZE; ++i, ++sample_idx) {
        frame.writeSample(sample_idx, _window[_read_index].readSample(i));
        }

        for (std::size_t i = 0; i < _overlap_index; ++i, ++sample_idx) {
            frame.writeSample(sample_idx, _window[(_read_index + 1) % AUDIO_WINDOW_SIZE].readSample(i));
        }
        /*remove frame as it is not needed anymore*/
        (void)popFrame();
    }

    return frame;
}

void AudioBuffer::resetOverlap() {
    _overlap_index = 0U;
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
