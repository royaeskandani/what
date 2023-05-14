// Roya Elisa Eskandani
// 10. February 2023 10:08

/* Possible outputs:
  Y : Gussed word is 'yes'
  N : Gussed word is 'no'
  Q : Not sure if it is 'yes' or 'no'
  q : INPUT.WAV is more than 2sec
*/

#include "environment.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define SAMPLING_RATE 25600
#define FEATURES 2
//*NOTE: FEATURES != 2 -> Change code B_inv in Calculation Moore–Penrose inverse

#define THRESHOLD_AMP 209
#define THRESHOLD_AMP_FAC 0.03
#define THRESHOLD_EDGE_LEN 2500
#define THRESHOLD_EDGE_FAC 250

#define THRESHOLD_FREQ 2000
#define THRESHOLD_GUESS 0.07

int16_t audio_0[1536000]; // stores 60 seconds of audio @ 25600 Hz, samples are 16 bit signed integers
int16_t audio_1[1536000];
int16_t audio_2[1536000];
int16_t audio_3[1536000];

uint32_t audio_length[WAVFILES]; // the array entries store the amount of samples each file has

char *filepath_0 = "DATASET.WAV"; // IOs: "project/DATASET.WAV" //! Windows: "DATASET.WAV"
char *filepath_1 = "INPUT.WAV"; // IOs: "project/INPUT.WAV" //! Windows: "INPUT.WAV"
char *filepath_2 = "";
char *filepath_3 = "";

uint32_t image[640][520]; // access: image[x][y], where image[0][0] ist the top left pixel

const float a0 = 0.9862117951198142;
const float a1 = -1.9724235902396283;
const float a2 = a0;
const float b1 = -1.972233470205696;
const float b2 = 0.9726137102735608;

// function: filter
void HPF();

// function: word extraction
void kill_silence(float max);
void extract_word(float max);

// function: pseudo inverse
float* calculate_weights();

// function: visualization
void ppm();
void print_frequnces(float* f);










int project(void) {
// 1. High pass filter and word extraction
  HPF();

  float max_amplitude = 0;
  for (int t = 0; t < audio_length[2]; t++) {
    int sample = audio_2[t];
    if (sample < 0) sample -= sample;
    
    if (sample > max_amplitude) max_amplitude = sample;
  }
  extract_word(max_amplitude);
  
  if (audio_length[3] > SAMPLING_RATE * 2) return 0; // 2 sec

// 2. Save extracted word
  save_audiofile("OUTPUT.WAV", 3, audio_length[3]); // IOs: "project/OUTPUT.WAV" //! Windows: "OUTPUT.WAV"
  // ppm();


// 3. Calculation Fourier
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


// 4. Calculation of the values for comparison:
// 4.1. Value: frequency (max magnitude)
  float max_magnitude = 0.;
  int freq_max_magnitude = 0;
  for (int i = 0; i < SAMPLING_RATE/2; i++) {
    if (max_magnitude < f[i]) {
      max_magnitude = f[i];
      freq_max_magnitude = i;
    }
  }

// 4.2. Value: ratio of sum magnitude < THRESHOLD_FREQ and sum magnitude > THRESHOLD_FREQ
  float under = 0., over = 0.;
  float ratio_magnitude;

  for (int i = 0; i < SAMPLING_RATE/2; i++) {
    if (i < THRESHOLD_FREQ) under += f[i];
    else over += f[i];
  }
  ratio_magnitude = under/over;


// 5. Calculation of Moore–Penrose inverse
  float* x = calculate_weights();


// 6. Return Guess  
  float value = (float) (x[0] * freq_max_magnitude + x[1] * ratio_magnitude * 200) /1000;
  free(x);

  if (value > THRESHOLD_GUESS) return 'Y'; // 89
  else if (value < -THRESHOLD_GUESS) return 'N'; // 78
  return 'Q'; // 81
}










void HPF() {
  // Quelle: LOUDNESS
  for(int t = 0; t < 512; t++) audio_1[t] = t * audio_1[t] / 512; // 20ms fade in

  float DELAY[2] = {0.0, 0.0};
  float out = 0;
  audio_2[0] = audio_1[0];
  audio_2[1] = audio_1[1];
  for(int t = 2; t < audio_length[1]; t++) {
    out  = a0 * audio_1[t] + a1 * audio_1[t-1] + a2 * audio_1[t-2];
    out -=                   b1 * DELAY[0]     + b2 * DELAY[1];
    DELAY[1] = DELAY[0];
    DELAY[0] = out;
    audio_2[t] = (int) (out + 0.5);
  }
  for (int t = 0; t < audio_length[1]; t++)
    audio_2[t] = audio_1[t];

  for(int t = 0; t < 700; t++) audio_2[t] = 0; // kill artefacts
  audio_length[2] = audio_length[1];
}


void kill_silence(float max) {
// 1. kill silence (start)
  for (int t = 0; t < audio_length[2]; t++) {
    int sample = audio_2[t];
    if (sample < 0) sample = -sample;

    if (sample > (float) max * THRESHOLD_AMP_FAC) break;
    audio_2[t] = 0;
  }
  
// 2. kill silence (end)
  for (int t = audio_length[2]; t > 0 ; t--) {
    int sample = audio_2[t];
    if (sample < 0) sample = -sample;

    if (sample > (float) max * THRESHOLD_AMP_FAC) break;
    audio_2[t] = 0;
  }
}


void extract_word(float max) {
  kill_silence(max);

// 1. extract sound
  int start = 0, end = audio_length[2];
  while (audio_2[start] == 0) start++;
  while (audio_2[end - 1] == 0) end--;

  int extraced_length = 0;
  for (int t = start; t < end; t++) {
    audio_3[extraced_length] = audio_2[t];
    audio_2[extraced_length] = audio_2[t]; // shift audio 2
    extraced_length++;
  }
  for (int t = extraced_length; t < end; t++) {
    audio_3[t] = 0;
    audio_2[t] = 0;
  }

  audio_length[2] = extraced_length;
  audio_length[3] = audio_length[2];


// 2. check edges
// 2.1. check start
  int sum = 0;
  for (int t = 0; t < THRESHOLD_EDGE_LEN; t++) {
    int sample = audio_3[t];
    if (audio_3[t] < 0 ) sum -= sample;
    else sum += sample;
  }
  int mean = sum/THRESHOLD_EDGE_LEN;
  if ((mean < (float) max * THRESHOLD_AMP_FAC) || (mean < THRESHOLD_AMP)) {
    for (int t = 0; t < THRESHOLD_EDGE_LEN/THRESHOLD_EDGE_FAC; t++)
      audio_2[t] = 0; 
    extract_word(max);
  }

// 2.2 check the end
  sum = 0;
  for (int t = 0; t < THRESHOLD_EDGE_LEN; t++) {
    int sample = audio_3[audio_length[3] - 1 - t];
    if (audio_3[audio_length[3] - 1 - t] < 0 ) sum -= sample;
    else sum += sample;
  }
  mean = sum/THRESHOLD_EDGE_LEN;
  if ((mean < (float) max * THRESHOLD_AMP_FAC) || (mean < THRESHOLD_AMP)) {
    for (int t = 0; t < THRESHOLD_EDGE_LEN/THRESHOLD_EDGE_FAC; t++)
      audio_2[audio_length[2] - 1 - t] = 0;
    audio_length[2] -= THRESHOLD_EDGE_LEN/THRESHOLD_EDGE_FAC;
    audio_length[3] = audio_length[2];
    extract_word(max);
  }
}


float* calculate_weights() {
  const int rows = audio_length[0] / (FEATURES + 1); // Number of entries of the data set
  const int columns = FEATURES;

  struct linearsystem* dataset = (struct linearsystem*) malloc(sizeof(linearsystem));
  dataset->A = (float*) malloc(sizeof(float) * rows * columns);  // Values of the dataset
  dataset->b = (int*) malloc(sizeof(int) * rows); // Targets of the dataset

  for (int m = 0; m < rows; m++) {
    for (int n = 0; n < columns; n++)
      dataset->A[m * columns + n] = audio_0[m * (FEATURES + 1) + n];
    dataset->b[m] = audio_0[m * (FEATURES + 1) + FEATURES];
  }

// 1. Calculation Moore–Penrose inverse
  // T_t: Transpose of A
  float A_t[columns][rows];
  for (int n = 0; n < columns; n++) {
    for (int m = 0; m < rows; m++) 
      A_t[n][m] = dataset->A[m * columns + n];
  }

  // B = A_t * A
  float B[columns][columns];
  for (int m = 0; m < columns; m++) {
    for (int n = 0; n < columns; n++) {
      B[m][n] = 0;
      for (int i = 0; i < rows; i++)
        B[m][n] += A_t[m][i] * dataset->A[i * rows + n];
    }
  }

  // B_inv = (A_t * A)^-1 //! Only for FEATURES == 2
  float B_inv[columns][columns];
  float det_M = B[0][0]*B[1][1] - B[0][1]*B[1][0];
  B_inv[0][0] = (float) 1/det_M * B[1][1];
  B_inv[0][1] = (float) 1/det_M * -B[0][1];
  B_inv[1][0] = (float) 1/det_M * -B[1][0];
  B_inv[1][1] = (float) 1/det_M * B[0][0];

  // A_inv = M_inv * A_t: Moore–Penrose inverse
  float A_inv[columns][rows];
  for (int m = 0; m < columns; m++) {
    for (int n = 0; n < rows; n++) {
      A_inv[m][n] = 0;
      for (int i = 0; i < columns; i++)
        A_inv[m][n] += B_inv[m][i] * A_t[i][n];
    }
  }


// 2. Calculation: Weights
  float* x = (float*) malloc(sizeof(float) * FEATURES);

  for (int m = 0; m < columns; m++) {
    x[m] = 0;
    for (int i = 0; i < rows; i++) {
      x[m] += A_inv[m][i] * dataset->b[i];
    }
  }

  free(dataset->b);free(dataset->A);free(dataset);
  return x;
}


void ppm() {
  for(int y=0; y<520; y++) {
  for(int x=0; x<640; x++)
    set_pixel(x, y, 0, 0, 255);  // blue
  }

  int16_t LOUD[640];
  for(int idx=0; idx<640; idx++)
    LOUD[idx] = 0;
    
  for(int t=0; t < audio_length[3]; t++) {
    int idx = t * 640 / audio_length[3];
    int sample = audio_3[t];
    if(sample < 0) sample = -sample;
    if(sample > LOUD[idx]) LOUD[idx] = sample;
  }

  for(int x=0; x<640; x++) {
    int level = 519 - LOUD[x] / 64;
    for(int y=level; y<520; y++)
    set_pixel(x, y, 255, 255, 0); // yellow level on blue screen
  }
  save_imagefile("OUTPUT.PPM"); // IOS: "project/OUTPUT.PPM" //! Windows: "OUTPUT.PPM"
}


void print_frequnces(float* f) {
  // Output for Excel
  for (int i = 0; i < audio_length[3]; i++)
    printf("%.4f\n", f[i]);
}