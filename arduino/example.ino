#include "AudioController.h"

AudioController sc;

void setup()
{
    // inicjalizacja wejść
    sc.initInput(2); // ustawienie pinu dla potencjometru

    // inicjalizacja wyjść
    int outputs[] = {4, 5, 6, 7};
    sc.initOutputs(outputs); // piny dla diód
}

void loop()
{
    int volume = sc.getInputVal();

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // tutaj trzeba wysłać zmienną volume do hosta
    int volumeHost = 50; // wartość 50 podmienić na odebraną z hosta
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    sc.setOutputs(volumeHost); // metoda przyjmuje argument z zakresu [0, 100]

    delay(10);
}
