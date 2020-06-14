#include "AudioController.h"
#include "message.h"
#include "codec.h"

AudioController sc;

void setup()
{
    // inicjalizacja wejść
    //sc.initInput(2); // ustawienie pinu dla potencjometru

    // inicjalizacja wyjść
    //int outputs[] = {4, 5, 6, 7};
    //sc.initOutputs(outputs); // piny dla diód
    Serial.begin(9600);
}

void loop()
{
    Message mes(HSND,"CHUJ");
    String coded = codec_encode(mes);
    for(char c : coded){
      Serial.write(c);
      delay(1);
    }
    delay(500);
}
