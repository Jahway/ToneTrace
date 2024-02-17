#include <stdio.h>
#include <stdlib.h>

#include <portaudio.h>

static void checkErr(PaError err){
    if (err != paNoError){
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

int main(){
    PaError err;
    err = Pa_Initialize();
    checkErr(err);

    printf("yooo\n");
    
    err = Pa_Terminate();
    checkErr(err);

    return EXIT_SUCCESS;
}