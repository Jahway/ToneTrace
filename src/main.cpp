/* Name: Jiawei Chen
* Netid: jchen125
* COSC: 594 Lab 1-Jgraph
* Mar 3rd, 2024
*
* This program will output time domain and frequency domain graph jgraph command file based on input audio file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "sndfile.hh"
#include "fftw3.h"

#define SPECTRO_FREQ_START 20
#define SPECTRO_FREQ_END 20000
#define BUFFER_LEN 1024
#define MAX_CHANNELS 2
#define FFT_SIZE 512
#define SPECTRO_FREQ_START 20
#define SPECTRO_FREQ_END 20000

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct
{
	double target_second;  // Target time for frequency graph
	const char* base_name; // File name without path
	SNDFILE* snd_file;
	SF_INFO sf_info;	  
}audio_data;

//Read audio file using libsndfile, extract audio data into input_buf and initialize audio_data struct
void read_audio(const char *in_file, audio_data *spectro_data, double **input_buf) {
	memset(&(spectro_data->sf_info), 0, sizeof(SF_INFO));

    if (!(spectro_data->snd_file = sf_open(in_file, SFM_READ, &spectro_data->sf_info))) {
        printf("Not able to open input file %s.\n", in_file);
        puts(sf_strerror(NULL));
        exit(1);
    }

    if (spectro_data->sf_info.channels > MAX_CHANNELS) {
        printf("Not able to process more than %d channels\n", MAX_CHANNELS);
        sf_close(spectro_data->snd_file);
        exit(2);
    }

	// Extracting just the file name from the path
    spectro_data->base_name = strrchr(in_file, '/');
    if (spectro_data->base_name) {
        spectro_data->base_name++; 
    } else {
        spectro_data->base_name = in_file; 
    }

    *input_buf = (double *)malloc(sizeof(double) * spectro_data->sf_info.frames * spectro_data->sf_info.channels);
    sf_readf_double(spectro_data->snd_file, *input_buf, spectro_data->sf_info.frames);
    sf_close(spectro_data->snd_file);
}



void time_domain(const audio_data* spectro_data, double *input_buf) {
    FILE *file = fopen("src/time_domain.jgr", "w");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    int frames = spectro_data->sf_info.frames;
    double duration = frames / (double)spectro_data->sf_info.samplerate;
    int target_points = 600;
    int downsampling_rate = MAX(1, frames / target_points);

    fprintf(file, "newgraph\n\n");
    fprintf(file, "xaxis\n  min 0 max %f\n  size 7\n  mhash 1\n  label : Time (s)\n\n", duration);
    fprintf(file, "yaxis\n  min 0 max 1\n  size 4\n  hash 0.1\n  hash_labels\n  mhash 5\n  label : Amplitude\n\n");

    fprintf(file, "newline color 0 0 .75\n");

    for (int i = 0; i < target_points; i++) {
        double maxAmplitude = 0.0; 
        for (int j = 0; j < downsampling_rate && (i * downsampling_rate + j) < frames; j++) {
            int idx = i * downsampling_rate + j;
            double currentAmplitude = fabs(input_buf[idx]); 
            if (currentAmplitude > maxAmplitude) {
                maxAmplitude = currentAmplitude; 
            }
        }

        double normalizedAmplitude = maxAmplitude;
        double time = (i * downsampling_rate) / (double)spectro_data->sf_info.samplerate;
        fprintf(file, "  pts %.3f %.2f\n", time, normalizedAmplitude);
    }

	fprintf(file, "newstring hjl vjt x 0.02 y 1.2 fontsize 10 : FileName:%s\n", spectro_data->base_name);
    fprintf(file, "newstring hjl vjt x 0.02 y 1.15 fontsize 10 : Sample rate:%d Hz\n", spectro_data->sf_info.samplerate);
    fprintf(file, "newstring hjl vjt x 0.02 y 1.1 fontsize 10 : Channels:%d\n", spectro_data->sf_info.channels);
    fprintf(file, "newstring hjl vjt x 0.02 y 1.05 fontsize 10 : Duration:%.1f seconds\n", ceil(duration));

    fclose(file);
}

// Function to apply a Hanning window to reduce spectral leakage
void apply_hanning_window(double *data, int size) {
    for (int i = 0; i < size; i++) {
        data[i] *= 0.5 * (1 - cos(2 * M_PI * i / (size - 1)));
    }
}

void frequency_domain(audio_data* spectro_data, double *input_buf) {
    int start_index = spectro_data->target_second * spectro_data->sf_info.samplerate;
    double *windowed_data = (double*) malloc(sizeof(double) * FFT_SIZE);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (FFT_SIZE/2 + 1));
    fftw_plan p = fftw_plan_dft_r2c_1d(FFT_SIZE, windowed_data, out, FFTW_ESTIMATE);

    FILE *file = fopen("src/frequency_domain.jgr", "w");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    memset(windowed_data, 0, sizeof(double) * FFT_SIZE);
    for (int i = 0; i < FFT_SIZE; i++) {
        int index = MIN(start_index + i, spectro_data->sf_info.frames - 1);
        windowed_data[i] = input_buf[index * spectro_data->sf_info.channels];
    }
    apply_hanning_window(windowed_data, FFT_SIZE);
    fftw_execute(p);

	fprintf(file, "newgraph\n\n");
    fprintf(file, "xaxis\n  min %d max %d\n  mhash 500\n  hash_labels\n  size 7\n  label : Frequency (Hz) at %.2f second\n\n", SPECTRO_FREQ_START, SPECTRO_FREQ_END, spectro_data->target_second);
    fprintf(file, "yaxis\n  min -120 max 0\n  hash 20\n  precision 0\n  size 4\n  label : Magnitude (dB)  \n\n");
    fprintf(file, "newline color 0 0 .75\n");

    for (int i = 0; i < FFT_SIZE/2; i++) {
        double freq = i * (double)spectro_data->sf_info.samplerate / FFT_SIZE;
        if (freq >= SPECTRO_FREQ_START && freq <= SPECTRO_FREQ_END) {
            double magnitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
            double magnitude_dB = 20 * log10(magnitude / FFT_SIZE);
            magnitude_dB = MAX(magnitude_dB, -120);  
            fprintf(file, "  pts %.2f %.2f\n", freq, magnitude_dB);
        }
    }

	fprintf(file, "newstring hjl vjt x 0.02 y 14\n  fontsize 10 : FileName:%s\n", spectro_data->base_name);
    fprintf(file, "newstring hjl vjt x 0.02 y 9\n  fontsize 10 : Sample rate:%d Hz\n", spectro_data->sf_info.samplerate);
    fprintf(file, "newstring hjl vjt x 0.02 y 4\n  fontsize 10 : Channels:%d\n", spectro_data->sf_info.channels);

    fclose(file);
    fftw_destroy_plan(p);
    fftw_free(out);
    free(windowed_data);
}


int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s -t <filename> or %s -f <filename> <second>\n", argv[0], argv[0]);
        return 1;
    }

    char *mode = argv[1]; 
    char *filename = argv[2];
    audio_data *spectro_data = (audio_data *)malloc(sizeof(audio_data));
    double *input_buf = NULL;

    if (strcmp(mode, "-t") == 0) {
        if (argc != 3) {
            printf("Usage: %s -t <filename>\n", argv[0]);
            free(spectro_data);
            return 1;
        }
        read_audio(filename, spectro_data, &input_buf);
        time_domain(spectro_data, input_buf);
    } 
	else if (strcmp(mode, "-f") == 0) {
        if (argc != 4) {
            printf("Usage: %s -f <filename> <second>\n", argv[0]);
            free(spectro_data);
            return 1;
        }

        spectro_data->target_second = atof(argv[3]);
		read_audio(filename, spectro_data, &input_buf);

        if (spectro_data->target_second < 0 || spectro_data->target_second * spectro_data->sf_info.samplerate >= spectro_data->sf_info.frames) {
            fprintf(stderr, "Invalid second specified.\n");
            free(spectro_data);
            free(input_buf);
            return 1;
        }
        
        frequency_domain(spectro_data, input_buf);
    } 
	else {
        printf("Invalid mode. Use -t for time domain analysis or -f for frequency domain analysis.\n");
        free(spectro_data);
        return 1;
    }

    free(input_buf);
    free(spectro_data);

    return 0;
}