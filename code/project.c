#include "environment.h"
#include <stdio.h>

#define THRESHOLD_AMP 180
#define THRESHOLD_GUESS 0.15
#define SKALE_FACTOR 100
//! ACHTUNG ggf. bearbeiten: Skalierungsfaktor wird nur beim rel_max_pos beim Erstellen eingerechnet. Da aus vorhergehenden Versionen eine Skalierung von 1/100 für max sinnvoll erschien. Darum habe ich (erstmal) die beiden Faktoren gleichgewählt.

int16_t audio_0[1536000]; // stores 60 seconds of audio @ 25600 Hz, samples are 16 bit signed integers
int16_t audio_1[1536000];
int16_t audio_2[1536000];
int16_t audio_3[1536000];

uint32_t audio_length[WAVFILES]; // the array entries store the amount of samples each file has

char *filepath_0 = "DATASET.WAV";
char *filepath_1 = "INPUT.WAV";
char *filepath_2 = "";
char *filepath_3 = ""; // OUTPUT.WAV


uint32_t image[640][520]; // access: image[x][y], where image[0][0] ist the top left pixel

const float a0 = 0.9862117951198142;
const float a1 = -1.9724235902396283;
const float a2 = a0;
const float b1 = -1.972233470205696;
const float b2 = 0.9726137102735608;

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

  for(int t = 0; t < 700; t++) audio_2[t] = 0; // kill artefacts
}

void killing_silence() {
  // killing silence at the beginning
  int flag = 0;
  for (int t = 0; t < audio_length[1]; t++) {
    int sample = audio_2[t];
    if (sample < 0) sample = -sample;

    if (sample > THRESHOLD_AMP) { flag = 1; break; }
    else if (flag == 0) audio_2[t] = 0;
  }
    
  
  // killing silence at the end
  flag = 0;
  for (int t = audio_length[1]; t > 0 ; t--) {
    int sample = audio_2[t];
    if (sample < 0) sample = -sample;

    if (sample > THRESHOLD_AMP) { flag = 1; break; }
    else if (flag == 0) audio_2[t] = 0;
  }
}

void extract_word() {
  killing_silence();
  
  int length_word = 0;
  for (int t = 0; t < audio_length[1]; t++) {
    if (audio_2[t] != 0) {
      audio_3[length_word] = audio_2[t];
      length_word++;
    }
  }

  audio_length[3] = length_word;
}

void print_guess(float b) {
  printf("b: %f\n", b);
  if (b > THRESHOLD_GUESS) printf("yes\n");
  else if (b < -THRESHOLD_GUESS) printf("no\n");
  else printf("I'm not sure.");
}


//!! MAIN
int project(void) {
// 1. High pass filter and word extraction
  HPF();
  extract_word();
  save_audiofile("OUTPUT.WAV", 3, audio_length[3]);
  # if 0 // OUTPUT.WAV visualization
  for(int y=0; y<520; y++)
    for(int x=0; x<640; x++)
      set_pixel(x, y, 0, 0, 255);  // blue

    int16_t LOUD[640];
    for(int idx=0; idx<640; idx++)
      LOUD[idx] = 0;
      
    for(int t=0; t<audio_length[1]; t++) {
      int idx = t * 640 / audio_length[1];
      int sample = audio_3[t];
      if(sample < 0) sample = -sample;
      if(sample > LOUD[idx]) LOUD[idx] = sample;
    }

    for(int x=0; x<640; x++) {
      int level = 519 - LOUD[x] / 64;
      for(int y=level; y<520; y++)
      set_pixel(x, y, 255, 255, 0); // yellow level on blue screen
    }
    save_imagefile("HPF.PPM");
  # endif


// 2. calculate parameters: max, rel_max_pos
  int max = 0, max_pos = 0;
  for (int t = 0; t < audio_length[3]; t++) {
    int sample = audio_3[t];
    if(sample < 0) sample = -sample;

    if (sample > max) { max = sample; max_pos = t; }
  }
  
  int rel_max_pos = SKALE_FACTOR * max_pos/audio_length[3];
  // printf("maximum: %d\nrel. maximum position: %d\n", max, rel_max_pos);


// 3. load dataset form DATASET.WAV
  int rows = audio_length[0] / 3;
  int columns = 3 - 1; // max, max_pos, yes/no (yes = 1, no = -1)
  // printf("rows: %d, columns: %d\n", rows, columns);

  struct parameters {
    float A[rows][columns];
    int b[rows];
  } parameters;

  for (int m = 0; m < rows; m++) {
    for (int n = 0; n < columns; n++)
      parameters.A[m][n] = (float) audio_0[3*m + n]/SKALE_FACTOR;
    parameters.b[m] = audio_0[3*m + 2];
  }
  # if 0
  printf("A: \n");
  for (int m = 0; m < rows; m++) {
    for (int n = 0; n < columns; n++)
      printf("%f ", parameters.A[m][n]);
    printf("\n");
  }

  printf("x: \n");
  for (int m = 0; m < rows; m++) 
    printf("%d ", parameters.b[m]);
  printf("\n");
  # endif
  

// 4. calculate pesudoinverse
  float A_t[columns][rows];
  for (int n = 0; n < columns; n++) {
    for (int m = 0; m < rows; m++) 
      A_t[n][m] = parameters.A[m][n];
  }
  # if 0
  printf("A_t: \n");
  for (int m = 0; m < columns; m++) {
    for (int n = 0; n < rows; n++) 
      printf("%f ", A_t[m][n]);
    printf("\n");
  }
  # endif

// 4.1. M = A_t * A
  float M[columns][columns];
  for (int m = 0; m < columns; m++) {
    for (int n = 0; n < columns; n++) {
      M[m][n] = 0;
      for (int i = 0; i < rows; i++) {
        M[m][n] += A_t[m][i] * parameters.A[i][n];
      }
    }
  }

  # if 0
  printf("M: \n");
  for (int m = 0; m < columns; m++) {
    for (int n = 0; n < columns; n++) 
      printf("%f ", M[m][n]);
    printf("\n");
  }
  # endif

// 4.2. M_inv = (A_t * A)^-1
  float M_inv[columns][columns];
  //! 2x2 Matrix
  float det_M = M[0][0]*M[1][1] - M[0][1]*M[1][0];
  M_inv[0][0] = (float) 1/det_M * M[1][1];
  M_inv[0][1] = (float) 1/det_M * -M[0][1];
  M_inv[1][0] = (float) 1/det_M * -M[1][0];
  M_inv[1][1] = (float) 1/det_M * M[0][0];
  # if 0
  printf("M_inv: \n");
  for (int m = 0; m < columns; m++) {
    for (int n = 0; n < columns; n++) 
      printf("%f ", M_inv[m][n]);
    printf("\n");
  }
  # endif

// 4.3. pseudoinverse: A_inv = M_inv * A_t
  float A_inv[columns][rows];
  for (int m = 0; m < columns; m++) {
    for (int n = 0; n < rows; n++) {
      A_inv[m][n] = 0;
      for (int i = 0; i < columns; i++)
        A_inv[m][n] += M_inv[m][i] * A_t[i][n];
    }
  }
  # if 0
  printf("A_inv: \n");
  for (int m = 0; m < columns; m++) {
      for (int n = 0; n < rows; n++) 
       printf("%f ", A_inv[m][n]);
     printf("\n");
  }
  # endif


// 5. guess INPUT.WAV

// 5.1. weights = Pseudoinverse * b
  float x[columns];
  for (int m = 0; m < columns; m++) {
    x[m] = 0;
    for (int i = 0; i < rows; i++) {
      x[m] += A_inv[m][i] * parameters.b[i];
    }
  }
  # if 0
  printf("weigths: \n");
  for (int i = 0; i < columns; i++)
    printf("%f ", x[i]);
  printf("\n");
  # endif

// 5.2. calculate decision
  float b = (float) x[0] * max / SKALE_FACTOR + x[1] * rel_max_pos / SKALE_FACTOR;
  // print_guess(b);

  if (b > THRESHOLD_GUESS) return 1; // yes
  else if (b < -THRESHOLD_GUESS) return -1; // no
  return 0; // not sure
}