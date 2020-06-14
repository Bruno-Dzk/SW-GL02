#include "AudioController.h"

AudioController sc;
int incomingByte;
int tmp;
//String tmp_s;
String help[10]={"ERDY","ASET","CSND","RSND","TSND","KEYP","VSND","HGET","HSND"};
int buttonState =0;


void setup()
{
    // inicjalizacja wejść
    sc.initInput(A0); // ustawienie pinu dla potencjometru
    for(int i=8;i<18;i++){
      pinMode(i, INPUT);
      }
    
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);

    // inicjalizacja wyjść
    int outputs[] = {4, 5, 6, 7};
    sc.initOutputs(outputs); // piny dla diód
    Serial.begin(9600);
//    stream = Stream();
}

void loop()
{

  for(int i=8;i<10;i++){
    buttonState=digitalRead(i);
      if (buttonState == HIGH) {
        for(int j=0;j<4;j++){
          Serial.write(help[i-8][j]);//powinno wyswietlić się na wyswietlaczu ale nie posiadaliśmy takiego więc jest przesłane do hosta 
          }
          delay(200);
    }


    int volume = sc.getInputVal();
      
      
      if (Serial.available() > 0) {
    incomingByte =Serial.read();// Stream.read();//
    Serial.write("-");
    if(isAscii(incomingByte)){
      Serial.write("asci");

      }
    Serial.write(incomingByte);
    if (incomingByte == 'R') {
      digitalWrite(1, HIGH);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
    }
    if (incomingByte == 'B') {
      digitalWrite(2, HIGH);
      digitalWrite(1, LOW);
      digitalWrite(3, LOW);
    }
    if (incomingByte == 'G') {
      digitalWrite(3, HIGH);
      digitalWrite(2, LOW);
      digitalWrite(1, LOW);
    }
  }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //Serial.println(volume);// tutaj trzeba wysłać zmienną volume do hosta
    int volumeHost = 0; // wartość 50 podmienić na odebraną z hosta
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   
    sc.setOutputs(volumeHost); // metoda przyjmuje argument z zakresu [0, 100]

    delay(10);

  }}
