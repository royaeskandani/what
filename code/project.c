/// 3 * 4 Audio-Beispiel
// 5/6 (Roya) + 3/4(Julian) + 3/4 (Seb) = 11/15 richtig = 73 %


#include "environment.h"
#include <stdlib.h>
#include <time.h>


int16_t audio_0[1536000]; // stores 60 seconds of audio @ 25600 Hz, samples are 16 bit signed integers
int16_t audio_1[1536000];
int16_t audio_2[1536000];
int16_t audio_3[1536000];

uint32_t audio_length[4]; // the array entries store the amount of samples each file has

char *filepath_0 = ""; // direct filename of audiofiles located in the "project" folder
char *filepath_1 = "";
char *filepath_2 = "";
char *filepath_3 = "";

// Bemerkung: 9 von 10 Testdateien haben funktioniert. Reicht, da es ja um "Fehler überspiele" geht.

int16_t absolute(int16_t value) {
    if (value > 0) return value;
    return -value;
}

char process_audio(int16_t *audio, int32_t audio_len) {
    int max = 0, pos_max = 0, len = 0;

    for (int i = 0; i < audio_len; i++) {
        if (absolute(audio[i]) > max)
            max = absolute(audio[i]);
    }
//TODO: Amplitude normieren? -> double bzw. float
    int threshold = 400;
    len = 0, pos_max = 0;
    for (int i = 0; i < audio_len; i++) {
        if (absolute(audio[i]) > threshold) {
            if (absolute(audio[i]) == max && pos_max == 0)
                pos_max = len;
            len++;
        }
    }
    
    if (pos_max > 1800) return 'N'; // int: 78
    return 'Y'; // int: 89
}

void reply(char answer, int x) {
    x %= 5;

    if (answer == 'N') {
        char* answers[] = {
            "No?! Okay, I got your answer.",
            "Okay, i log 'no' into my system.",
            "No it is.",
            "Sorry, I didn't pay attention.",
            "Did you say 'no'?"
        };

        printf("%s ", answers[x]);
    } else {
        char* answers[] = {
            "You said yes, right?!",
            "You're right. I also think 'yes' is the correct answer.",
            "I heard 'yes'.",
            "Hmmm.. yes, right?!",
            "I hope 'yes' is the right answer."
        };

        printf("%s ", answers[x]);
    }
}

int project(void) {
    time_t t;
    srand((unsigned) time(&t));
    int requests = rand() % 5;
    

    filepath_0 = "yes_julian.wav";


    for (int i = 0; i < requests % 3; i++) { // Gewichtung häufiger keiner oder einmal nachfragen als zweimal
        reply(process_audio(audio_0, audio_length[0]), rand());
        printf("Please repeat.\n");
    }
        
    printf("Solution: %c\n", process_audio(audio_0, audio_length[0]));
    return process_audio(audio_0, audio_length[0]);
}  