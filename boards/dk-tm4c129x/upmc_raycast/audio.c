#include <string.h>
#include <stdbool.h>
#include "audio.h"
#include "drivers/sound.h"

typedef struct
{
    Sound *snd;
    bool ping;
    bool pong;
    bool playing;
    bool loop;
    int8_t buf[2][AUDIO_BUFFER_SIZE];
} AudioContext;

static AudioContext ctx;

static void audio_soundCB(uint32_t bufId)
{
    // It would be possible to fill the buffers here when requested. However,
    // this callback is called from the interrupt that manages the PWM to
    // generate the waveform. To not deteriorate the sound quality, raise flags
    // here and manage buffer filling in the main program flow.
    if (bufId == 0)
        ctx.ping = true;
    else if (bufId == 1)
        ctx.pong = true;
}

static void audio_fill(int bufId)
{
    if (ctx.snd && ctx.playing) {
        int nextPos = ctx.snd->pos + AUDIO_BUFFER_SIZE;

        if (nextPos < ctx.snd->len) {
            // Stream ongoing. Copy data to the requested buffer.
            memcpy(&ctx.buf[bufId], &ctx.snd->data[ctx.snd->pos],
                AUDIO_BUFFER_SIZE);

            ctx.snd->pos = nextPos;
        }
        else {
            // End of stream. Copy the remaining data and fill with silence.
            int size = ctx.snd->len - ctx.snd->pos;

            memcpy(&ctx.buf[bufId], &ctx.snd->data[ctx.snd->pos], size);
            memset(&ctx.buf[bufId][size], 0, AUDIO_BUFFER_SIZE - size);

            audio_rewind();
            if (!ctx.loop)
                ctx.playing = false;
        }
    }
    else {
        // Fill with silence.
        memset(&ctx.buf[bufId], 0, AUDIO_BUFFER_SIZE);
    }
}

void audio_init(uint32_t sysclk)
{
    ctx.snd = 0;
    ctx.playing = false;
    ctx.loop = false;
    memset(ctx.buf, 0, sizeof(ctx.buf));

    SoundInit(sysclk);
    SoundStart((int16_t *)&ctx.buf,
        sizeof(ctx.buf) / 2, AUDIO_SAMPLE_RATE,
        audio_soundCB);
}

void audio_process(void)
{
    if (ctx.ping) {
        audio_fill(0);
        ctx.ping = false;
    }

    if (ctx.pong) {
        audio_fill(1);
        ctx.pong = false;
    }
}

void audio_play(Sound *sound, bool loop)
{
    if (!sound)
        return;

    ctx.snd = sound;
    ctx.loop = loop;
    audio_resume();
}

void audio_stop(void)
{
    audio_pause();
    audio_rewind();
}

void audio_resume(void)
{
    if (!ctx.snd)
        return;

    ctx.playing = true;
}

void audio_pause(void)
{
    ctx.playing = false;
}

void audio_rewind(void)
{
    if (ctx.snd)
        ctx.snd->pos = 0;
}
