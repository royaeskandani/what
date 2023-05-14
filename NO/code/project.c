// Roya Elisa Eskandani
// 10. February 2023 10:02

#include "environment.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TARGET -1 
//*NOTE: YES == 1, NO == -1

#define SAMPLING_RATE 25600

#define THRESHOLD_FREQ 2000

int16_t audio_0[1536000]; // stores 60 seconds of audio @ 25600 Hz, samples are 16 bit signed integers
int16_t audio_1[1536000];
int16_t audio_2[1536000];
int16_t audio_3[1536000];

uint32_t audio_length[WAVFILES]; // the array entries store the amount of samples each file has

char *filepath_0 = "DATASET.WAV"; // IOs: "project/DATASET.WAV"; //! Windows: "DATASET.WAV"
char *filepath_1 = "";
char *filepath_2 = "";
char *filepath_3 = "OUTPUT.WAV"; // IOs: "project/OUTPUT.WAV" //! Windows: "OUTPUT.WAV"










int project(void) {
// 1. Calculation Fourier
  float f[SAMPLING_RATE/2], X_re[SAMPLING_RATE/2], X_im[SAMPLING_RATE/2];

  // Frequency calculation with Fourier
  for (int i = 0; i < SAMPLING_RATE/2; i++) {
      X_re[i] = 0;
      X_im[i] = 0;
      for (int j = 0; j < audio_length[3]; j++) {
          float omega = (float) 2 * M_PI / SAMPLING_RATE;
          X_re[i] += audio_3[j] * cos(omega * j * i);
          X_im[i] += audio_3[j] * sin(omega * j * i);
      }
  }
  
  for (int i = 0; i < SAMPLING_RATE/2; i++)
      f[i] = sqrt(X_re[i] * X_re[i] + X_im[i] * X_im[i]) / SAMPLING_RATE;
  

// 2. Calculation of the values for comparison:
// 2.1. Value: frequency (max magnitude)
  float max_magnitude = 0.;
  int freq_max_magnitude = 0;
  for (int i = 0; i < SAMPLING_RATE/2; i++) {
    if (max_magnitude < f[i]) {
      max_magnitude = f[i];
      freq_max_magnitude = i;
    }
  }

// 2.2. Value: ratio of sum magnitude < THRESHOLD_FREQ and sum magnitude > THRESHOLD_FREQ
  float under = 0., over = 0.;
  float ratio_magnitude;

  for (int i = 0; i < SAMPLING_RATE/2; i++) {
    if (i < THRESHOLD_FREQ) under += f[i];
    else over += f[i];
  }
  ratio_magnitude = (float) under / over;


// 3. Update Dataset
  audio_length[0] += 3;
  audio_0[audio_length[0] - 3] = freq_max_magnitude;
  audio_0[audio_length[0] - 2] = (int) (ratio_magnitude * 200);
  audio_0[audio_length[0] - 1] = TARGET;

  save_audiofile("DATASET.WAV", 0, audio_length[0]);  // IOs: "project/DATASET.WAV"; //! Windows: "DATASET.WAV"

  return 0;
}