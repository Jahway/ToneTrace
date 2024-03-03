#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

typedef struct
{
	int frame_num;  // Number of elements in our FFT output to display from the start index
	SNDFILE* snd_file; // the file
	SF_INFO sf_info;	  // information about the file

} audio_data;

double* input_buf;

static int display(
	const float *inputBuffer,
	const int read_count, audio_data* spectro_data)
{

	return 0;
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
	
	int buf_size = spectro_data->sf_info.frames * spectro_data->sf_info.channels;
	FILE *file = fopen("test.txt", "w");
	for(int i = 0; i < buf_size; i++){
		fprintf(file, "%f\n", input_buf[i]);
	}
	fclose(file);  // Close the file


	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * spectro_data->sf_info.frames);
	p = fftw_plan_dft_r2c_1d(spectro_data->sf_info.frames, input_buf, out, FFTW_ESTIMATE);

    fftw_execute(p);
    fftw_destroy_plan(p); 
	free(input_buf); 
	
	free(spectro_data);

	return 0;
}