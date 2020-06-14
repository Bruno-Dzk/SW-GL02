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

int counter = 0;

void loop()
{
    Message mes(HSND,"CHUJ");
    String coded = codec_encode(mes);
    char * to_send = new char[coded.length()];
    coded.toCharArray(to_send, coded.length());
    Serial.write(to_send);
    counter += 1;
    delay(500);
}
