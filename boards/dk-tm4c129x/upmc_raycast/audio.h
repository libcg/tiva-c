#ifndef __AUDIO_H
#define __AUDIO_H

#include <stdint.h>

#define AUDIO_SAMPLE_RATE   (16000)
#define AUDIO_BUFFER_SIZE   (4096)

typedef struct
{
    int pos;
    int len;
    uint8_t *data;
} Sound;

void audio_init(uint32_t sysclk);
void audio_process(void);

void audio_play(Sound *sound, bool loop);
void audio_stop(void);
void audio_resume(void);
void audio_pause(void);
void audio_rewind(void);

#endif // __AUDIO_H
