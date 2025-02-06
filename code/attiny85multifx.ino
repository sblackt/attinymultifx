#define AUDIO_IN A3   // PB3 (ADC Input)
#define AUDIO_OUT 1   // PB1 (PWM Output)
#define FX_SELECT_POT A1 // PB4 (Potentiometer for effect selection)

void setup() {
    pinMode(AUDIO_IN, INPUT);
    pinMode(AUDIO_OUT, OUTPUT);
}

void loop() {
    int rawSample = analogRead(AUDIO_IN);       // Read audio input
    int potValue = analogRead(FX_SELECT_POT);   // Read pot value (0-1023)
    int effectIndex = potValue / 341;           // Map to 0, 1, or 2 for 3 effects

    // Process audio based on effectIndex
    int processedOutput;
    if (effectIndex == 0) {
        processedOutput = bitCrusher(rawSample);
    } else if (effectIndex == 1) {
        processedOutput = waveFolder(rawSample);
    } else {
        processedOutput = glitchDelay(rawSample);
    }

    analogWrite(AUDIO_OUT, processedOutput);    // Send processed audio to output
}

int bitCrusher(int sample) {
    int bitDepth = analogRead(A2) / 128 + 1;  // Map Level pot (PB5) to 1-8 bits
    return (sample >> bitDepth) << bitDepth;  // Reduce bit depth
}

int waveFolder(int sample) {
    static int waveFolderOutputPrev = 512;
    int foldAmount = analogRead(A2) / 256 + 1; // Map Level pot to fold amount
    int waveFolderOutput = abs(sample - 512) % (256 / foldAmount) * 2;
    waveFolderOutput = (waveFolderOutputPrev * 7 + waveFolderOutput) / 8; // Smoothing
    waveFolderOutputPrev = waveFolderOutput;
    return waveFolderOutput;
}

int glitchDelay(int sample) {
    static int delayBuffer[128];
    static int index = 0;

    int delayTime = analogRead(A2) / 8; // Map Level pot to delay time
    index = (index + 1) % (delayTime + 1);
    int delayedSample = delayBuffer[index];
    delayBuffer[index] = sample;
    return delayedSample;
}
