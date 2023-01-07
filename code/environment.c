#include "environment.h"

FILE *fp;
char *audiofile_path[30];
int16_t *audiobuffer_pointer[30];
union wav_file read_buffer;
union wav_file write_buffer;
uint8_t wav_header[44] = {0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x00, 0x00, 0x00, 0x00};
int16_t audio_temp[1536000];

void save_audiofile(char *filename, uint8_t id, uint32_t length) {
    for (uint32_t i = 0; i < 44; ++i) write_buffer.bytes[i] = wav_header[i]; // copy over the header

    uint32_t bytes_to_write = (length * 2) + 44; // 2 bytes per sample plus 44 bytes header
    // write_buffer.size_a = SWITCH_ENDIAN_32(bytes_to_write - 8); // update header field a according to audio length
    write_buffer.wav_fields.size_a = SWITCH_ENDIAN_32(bytes_to_write - 8); // update header field a according to audio length
    // write_buffer.size_b = SWITCH_ENDIAN_32(bytes_to_write - 44); // update header field a according to audio length
    write_buffer.wav_fields.size_b = SWITCH_ENDIAN_32(bytes_to_write - 44); // update header field a according to audio length

    int16_t *pointer = audiobuffer_pointer[id];
    // for (uint32_t i = 0; i < length; ++i) write_buffer.samples[i] = *pointer++;
    for (uint32_t i = 0; i < length; ++i) write_buffer.wav_fields.samples[i] = *pointer++;

    fp = fopen(filename, "wb");
    fwrite(write_buffer.bytes, 1, bytes_to_write, fp);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    audiofile_path[0] = filepath_0; // assign pointers of user defined filename strings to array of pointers
    audiofile_path[1] = filepath_1;
    audiofile_path[2] = filepath_2;
    audiofile_path[3] = filepath_3;

    audiobuffer_pointer[0] = audio_0; // assign pointers to audio sample arrays to array of pointers
    audiobuffer_pointer[1] = audio_1;
    audiobuffer_pointer[2] = audio_2;
    audiobuffer_pointer[3] = audio_3;

    for (uint32_t i = 0; i < 4; ++i) {
        fp = fopen(audiofile_path[i], "rb"); // open file in binary read mode

        if (fp) {
            fseek(fp, 0, SEEK_END);
            uint32_t file_length = ftell(fp); // determine file length
            fseek(fp, 0, SEEK_SET);

            if (file_length > 3072044) {
                printf("file %d \"%s\" error: file is longer than 3.072.044 bytes (60 seconds of .wav plus fileheader)\n", i, audiofile_path[i]);
            } else if (file_length >= 44) {
                if (fread(read_buffer.bytes, 1, file_length, fp) != file_length) { // attempt to read file into temporary buffer
                    printf("file %d \"%s\" error: file could not be read\n", i, audiofile_path[i]);
                } else {
                    audio_length[i] = (file_length - 44) / 2; // minus 44 header bytes, two bytes equal one sample
                    int16_t *pointer = audiobuffer_pointer[i];
                    // for (uint32_t j = 0; j < audio_length[i]; ++j) *pointer++ = read_buffer.samples[j]; // take only the audio samples from the temporary file buffer and write them to a sample array
                    for (uint32_t j = 0; j < audio_length[i]; ++j) *pointer++ = read_buffer.wav_fields.samples[j]; // take only the audio samples from the temporary file buffer and write them to a sample array
                    

                    printf("file %d \"%s\" ok: file length is %d samples\n", i, audiofile_path[i], audio_length[i]);
                }
            } else {
                printf("file %d \"%s\" error: file is empty or not a valid .wav file\n", i, audiofile_path[i]);
            }

            fclose(fp);
        } else {
            // printf("file %d \"%s\" error: file does not exist!\n", i, audiofile_path[i]);
        }
    }

    
    return project();
}
