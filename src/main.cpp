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
	int frame_num;  // Number of elements in our FFT output to display from the start index
	SNDFILE* snd_file; // the file
	SF_INFO sf_info;	  // information about the file

} audio_data;

double* input_buf;

void time_domain(const char* filename, const audio_data* spectro_data) {
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
    double hash_interval = duration / 10;
    fprintf(file, "xaxis\n  min 0 max %f\n  size 7\n  mhash 1\n  label : Time (s)\n\n", duration);
    fprintf(file, "yaxis\n  min 0 max 1\n  size 4\n  hash 0.1\n  hash_labels\n  mhash 5\n  label : Amplitude\n\n");

    fprintf(file, "newline color 0 0 .75\n");

    for (int i = 0; i < target_points; i++) {
        double maxAmplitude = 0.0; // Initialize with minimal value
        for (int j = 0; j < downsampling_rate && (i * downsampling_rate + j) < frames; j++) {
            int idx = i * downsampling_rate + j;
            double currentAmplitude = fabs(input_buf[idx]); // Get the absolute value for amplitude
            if (currentAmplitude > maxAmplitude) {
                maxAmplitude = currentAmplitude; // Capture the peak amplitude within the segment
            }
        }

        double normalizedAmplitude = maxAmplitude;
        double time = (i * downsampling_rate) / (double)spectro_data->sf_info.samplerate;
        fprintf(file, "  pts %.3f %.2f\n", time, normalizedAmplitude);
    }

	// Extracting just the file name from the path
    const char *base_name = strrchr(filename, '/');
    if (base_name) {
        base_name++; 
    } else {
        base_name = filename; 
    }

	fprintf(file, "newstring hjl vjt x 0.02 y 1.2 fontsize 10 : FileName:%s\n", base_name);
    fprintf(file, "newstring hjl vjt x 0.02 y 1.15 fontsize 10 : Sample rate:%d Hz\n", spectro_data->sf_info.samplerate);
    fprintf(file, "newstring hjl vjt x 0.02 y 1.1 fontsize 10 : Channels:%d\n", spectro_data->sf_info.channels);
    fprintf(file, "newstring hjl vjt x 0.02 y 1.05 fontsize 10 : Duration:%.1f seconds\n", ceil(duration));

    fclose(file);
}


void read_audio(char* in_file, audio_data* spectro_data)
{
	int read_count = 0;
	memset(&(spectro_data->sf_info), 0, sizeof(SF_INFO));

	if (!(spectro_data->snd_file = sf_open(in_file, SFM_READ, &spectro_data->sf_info)))
	{ 
		printf("Not able to open input file %s.\n", in_file);
		puts(sf_strerror(NULL));
		exit(1);
	}

	if (spectro_data->sf_info.channels > MAX_CHANNELS)
	{
		printf("Not able to process more than %d channels\n", MAX_CHANNELS);
		sf_close(spectro_data->snd_file);
		exit(2);
	}

	input_buf = (double*) malloc((sizeof(double)) * spectro_data->sf_info.frames * spectro_data->sf_info.channels);
	memset(input_buf, 0, (sizeof(double)) * spectro_data->sf_info.frames * spectro_data->sf_info.channels);
	
	
	read_count = sf_readf_double(spectro_data->snd_file, input_buf, spectro_data->sf_info.frames);

	sf_close(spectro_data->snd_file);
	
}

int main(int agrc, char **argv)
{
	fftw_plan p;
	fftw_complex *out;

	static double data[BUFFER_LEN];
	static double bartop[FFT_SIZE];
	audio_data* spectro_data = (audio_data*) malloc(sizeof(audio_data));

	if (agrc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		free(spectro_data);
		return 1;
	}
	
	read_audio(argv[1], spectro_data);

	time_domain(argv[1], spectro_data);

	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * spectro_data->sf_info.frames);
	p = fftw_plan_dft_r2c_1d(spectro_data->sf_info.frames, input_buf, out, FFTW_ESTIMATE);

    fftw_execute(p);

    fftw_destroy_plan(p);
	free(input_buf); 
	free(spectro_data);

	return 0;
}