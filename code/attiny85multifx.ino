#define AUDIO_IN A3   // PB3 (ADC Input, Pin 2)
#define FX_SELECT 0   // PB0 (Switch: Bit Crusher <-> Wave Folder, Pin 5)
#define AUDIO_OUT 1   // PB1 (PWM Output, Pin 6)
#define BLEND_CTRL A1 // PB2 (Blend knob, Pin 7)
#define LEVEL_LEFT A2 // PB4 (Effect Level: Crusher/Fold, Pin 3)
#define LEVEL_RIGHT A0 // PB5 (Effect Level: Delay, Pin 1)


bool useWaveFolder = false;

void setup() {
    pinMode(AUDIO_IN, INPUT);
    pinMode(AUDIO_OUT, OUTPUT);
    pinMode(FX_SELECT, INPUT_PULLUP);
}

void loop() {
    int rawSample = analogRead(AUDIO_IN); // Read input
    int blend = analogRead(BLEND_CTRL) / 4; // 0-255
    int levelLeft = analogRead(LEVEL_LEFT) / 4;
    int levelRight = analogRead(LEVEL_RIGHT) / 4;

    useWaveFolder = !digitalRead(FX_SELECT); // Read switch

    int processedLeft = useWaveFolder ? waveFolder(rawSample) : bitCrusher(rawSample);
    int processedRight = glitchDelay(rawSample);

    int mixedOutput = (processedLeft * (255 - blend) + processedRight * blend) / 255;
    analogWrite(AUDIO_OUT, mixedOutput);
}

int bitCrusher(int sample) {
    int bitDepth = analogRead(LEVEL_LEFT) / 128 + 1; // Map 0-1023 to 1-8 bits
    return (sample >> bitDepth) << bitDepth; // Reduce bit depth
}

int waveFolder(int sample) {
    int foldAmount = analogRead(LEVEL_LEFT) / 256 + 1; // Fold amount based on pot value
    static int waveFolderOutputPrev = 0; // Variable to store the previous output value

    // Apply folding and smoothing
    int waveFolderOutput = abs(sample - 512) % (256 / foldAmount) * 2;
    waveFolderOutput = (waveFolderOutputPrev * 7 + waveFolderOutput) / 8;
    waveFolderOutputPrev = waveFolderOutput;

    return waveFolderOutput;
}


int glitchDelay(int sample) {
    static int delayBuffer[128];
    static int index = 0;
    
    int delayTime = analogRead(LEVEL_RIGHT) / 8; // Variable delay
    index = (index + 1) % (delayTime + 1);
    int delayedSample = delayBuffer[index];
    delayBuffer[index] = sample;
    return delayedSample;
}
