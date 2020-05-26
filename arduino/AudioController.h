#pragma once
#define NUMBER_OF_OUTPUTS 4

////////////////////////////////////////////////////////////////////////////

class AudioController
{
public:
    AudioController() {}
    AudioController(int, int[NUMBER_OF_OUTPUTS]);

    void initInput(int pin) { this->inputPin = inputPin; } // potencjometr
    void initOutputs(int[NUMBER_OF_OUTPUTS]); // diody
    
    int getInputVal() { analogRead(inputPin); }
    void setOutputs(int); // ustawienie diód w zależności od głośności

private:
    int inputPin; // port potencjometru
    int outputPins[NUMBER_OF_OUTPUTS]; // porty diód
};

////////////////////////////////////////////////////////////////////////////

AudioController::AudioController(int inputPin, int outputPins[NUMBER_OF_OUTPUTS])
{
    initInput(inputPin);
    initOutputs(outputPins);
}

void AudioController::initOutputs(int pins[NUMBER_OF_OUTPUTS])
{
    for (int i = 0; i < NUMBER_OF_OUTPUTS; i++)
    {
        outputPins[i] = pins[i];
        pinMode(outputPins[i], OUTPUT);
        digitalWrite(outputPins[i], LOW);
    }
}

void AudioController::setOutputs(int volume) // głośność od 0 do 100
{
    int volStep = 100 / NUMBER_OF_OUTPUTS;
        
    for (int i = volStep / 2, j = 0; i <= 100; i += volStep, j++)
    {            
        if (i >= volume)
            digitalWrite(outputPins[j], HIGH);
        else
            digitalWrite(outputPins[j], LOW);
    }
}

////////////////////////////////////////////////////////////////////////////

#undef NUMBER_OF_OUTPUTS
