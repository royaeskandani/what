#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#define WAVFILES 4

#include <inttypes.h>
#include <stdio.h>

#include "project.h"

union wav_file {
    struct wav_fields {
        uint8_t header[4];
        uint32_t size_a;
        uint8_t more_header[32];
        uint32_t size_b;
        int16_t samples[1536000];
    };
    uint8_t bytes[3072044];
};

extern int16_t audio_0[1536000];
extern int16_t audio_1[1536000];
extern int16_t audio_2[1536000];
extern int16_t audio_3[1536000];

extern uint32_t audio_length[WAVFILES];

extern char *filepath_0;
extern char *filepath_1;
extern char *filepath_2;
extern char *filepath_3;

extern uint32_t image[640][520];

void set_pixel(uint32_t x, uint32_t y, uint8_t R,  uint8_t G,  uint8_t B);

void save_imagefile(char *filename);

void save_audiofile(char *filename, uint8_t id, uint32_t length);

#endif
