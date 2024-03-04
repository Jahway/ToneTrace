# ToneTrace

ToneTrace is an audio analysis tool. The tool is designed to analyze audio files and generate corresponding time domain and frequency domain graphs. It utilizes the  `jgraph` for graph generations, `libsndfile` library for audio file processing and `fftw3` for fast Fourier transform computations.

## Features

- Generates time domain graphs to visualize audio waveforms over time.
- Produces frequency domain graphs to display the frequency spectrum of audio signals.
- Supports various audio file formats.
- Capable of handling audio files with different sampling rates and channel counts.

## Prerequisites

- [libsndfile](http://www.mega-nerd.com/libsndfile/): An audio library for reading and writing files containing sampled sound.
- [FFTW](http://www.fftw.org/): A C library for computing the discrete Fourier transform (DFT).

## Installation

1. Clone the ToneTrace repository.
2. Navigate to the ToneTrace directory.
3. Compile the program by running:

   ```
   make
   ```

## Usage
To Jacob:
Please run:
```
sh grade.sh
```

To generate time domain and frequency domain graphs for an audio file, use the following command:

```
./tonetrace -t <audio_file>
./tonetrace -f <audio_file> <target_second>
```

- `-t`: Generates a time domain graph for the specified audio file.
- `-f`: Produces a frequency domain graph for the audio file at the specified second.

## Output

The program outputs `.jgr` files, which can be converted to graphical format using Jgraph. For instance, to convert a Jgraph file to a PDF, you can use the following command:

```
jgraph -P file.jgr | ps2pdf - > output.pdf
```


## Acknowledgements

- Dr. James S. Plank at University of Tennessee for providing the opportunity to develop this project.
- [libsndfile](http://www.mega-nerd.com/libsndfile/) and [FFTW](http://www.fftw.org/) libraries for enabling audio processing and Fourier transform capabilities.
