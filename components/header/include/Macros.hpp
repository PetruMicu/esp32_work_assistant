#ifndef MACROS_H
#define MACROS_H

#include "AudioInput.hpp"
#include "driver/i2s_std.h"

/*Sound sampling parameters*/
#define AUDIO_BUFFER_SIZE 512 /*number of samples*/
#define AUDIO_POLLING_TIME ((AUDIO_BUFFER_SIZE / (SAMPLE_RATE / 1000)) * 1.1) /*time needed to gather enough samples*/
#define AUDIO_SOUND_DURATION 1 /*duration in seconds*/
#define AUDIO_WINDOW_BUFFER_SIZE (SAMPLE_RATE / AUDIO_BUFFER_SIZE + 5U)

#endif /*MACROS_H*/