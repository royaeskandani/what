#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "project.h"

union wav_file {
    struct wav_fields {
        uint8_t header[4];
        uint32_t size_a;
        uint8_t more_header[32];
        uint32_t size_b;
        int16_t samples[1536000];
    } wav_fields;
    uint8_t bytes[3072044];
};

extern int16_t audio_0[1536000];
extern int16_t audio_1[1536000];
extern int16_t audio_2[1536000];

extern uint32_t audio_length[3];

extern char *filepath_0;
extern char *filepath_1;
extern char *filepath_2;

#define SWITCH_ENDIAN_32(x) ((0x000000FF & (x)) << 24) | ((0x0000FF00 & (x)) << 8) | ((0x00FF0000 & (x)) >> 8) | ((0xFF000000 & (x)) >> 24)

void save_audiofile(char *filename, uint8_t id, uint32_t length);

#endif
