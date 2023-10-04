
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <portaudio.h>

static int stream_callback(const void *inputBuffer,
                           void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData) {
    static const int NUMBER_CHANNELS = 2;
    memcpy(outputBuffer, inputBuffer, framesPerBuffer * NUMBER_CHANNELS * sizeof(short));
    return paContinue;
}

int main(int argc, char *argv[]) {
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) goto ERROR_INIT;

    PaStreamParameters inputParameters = {
        .device = Pa_GetDefaultInputDevice(),
        .channelCount = 2,
        .sampleFormat = paInt16,
        .suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency,
        .hostApiSpecificStreamInfo = NULL,
    };
    PaStreamParameters outputParameters = {
        .device = Pa_GetDefaultOutputDevice(),
        .channelCount = 2,
        .sampleFormat = paInt16,
        .suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency,
        .hostApiSpecificStreamInfo = NULL,
    };

    const int sample_rate = 44100;
    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        &outputParameters,
                        sample_rate,
                        paFramesPerBufferUnspecified,
                        paNoFlag,
                        stream_callback,
                        NULL);

    if (err != paNoError) goto ERROR;

    err = Pa_StartStream(stream);
    if (err != paNoError) goto ERROR_STREAM;

    const int duration_seconds = 10;
    sleep(duration_seconds);

    err = Pa_StopStream(stream);
    if (err != paNoError) goto ERROR_STREAM;

    err = Pa_CloseStream(stream);
    if (err != paNoError) goto ERROR;

    Pa_Terminate();
    return 0;

ERROR_STREAM:
    Pa_CloseStream(stream);
ERROR:
    Pa_Terminate();
ERROR_INIT:
    fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    return 1;
}
