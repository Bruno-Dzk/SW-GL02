#include "AudioController.h"
#include "message.h"
#include "codec.h"

AudioController audio_controller;

int potPin = 2;    // select the input pin for the potentiometer
int ledPin = 13;   // select the pin for the LED
unsigned long val = 0;       // variable to store the value coming from the sensor
int t1 = 0;
int aset_acc = 0;

void setup()
{
    // inicjalizacja wejść
    //sc.initInput(2); // ustawienie pinu dla potencjometru

    // inicjalizacja wyjść
    int outputs[] = {4, 5, 6, 7};
    //sc.initOutputs(outputs); // piny dla diód
    audio_controller =AudioController(2,outputs);
    Serial.begin(9600);
}

void send_message(){
  val = analogRead(2);
  Message mes(ASET,val);
  size_t datasize;
  byte * encoded= codec_encode(mes, datasize);
  for(int i = 0; i < datasize; i++){
      Serial.write(encoded[i]);
  }
  delete [] encoded;
  /*Serial.print(val);
  Serial.print(' ');
  for(int i = 0; i < datasize; i++){
    Serial.print(int(encoded[i]));
    Serial.print(' ');
  }
  Serial.print('\n');*/
}

unsigned long counter = 0;
bool host_ready = false;

void loop()
{
    byte messageStart;
    do {
      messageStart = Serial.read();
      if(messageStart != 255){
        Serial.println(int(messageStart)); 
      }
    } while(messageStart != 254);

    // read header
    int read_for_header = 0;
    byte header [4];
    do {
        byte read_byte = Serial.read();
        if(read_byte != 255){
          header[read_for_header] = read_byte;
          read_for_header += 1;
        }
    } while(read_for_header < 4);
    for(int i = 0; i < 4; i++){
      Serial.print(char(header[i]));
      Serial.print(' ');
    }
    Serial.print('\n');

    int no_of_bytes;
    if(false){
        no_of_bytes = Serial.read();
    }else{
        no_of_bytes = 5;
    }       

    byte * data = new byte[no_of_bytes];
    int read_for_data = 0;
    do {
        byte read_byte = Serial.read();
        if(read_byte != 255){
          data[read_for_data] = read_byte;
          read_for_data += 1;
        }
    } while(read_for_data < no_of_bytes);
    /*(for(int i = 0; i < no_of_bytes; i++){
      Serial.print(int(data[i]));
      Serial.print(' ');
    }
    Serial.print('\n');*/
    
    byte control_sum = Serial.read();
    Message test = codec_decode(header, data, no_of_bytes);
    Serial.println(test.text);
    
    delete [] data;
  /*if(!host_ready){
    delay(5000);
    host_ready = true;
  }*/
  // read the incoming byte:
  
  delay(50);
}

void comment()
{
  int t2 = millis();
    int delta = t2 - t1;
               // stop the program for some time*/  
    t1 = t2;
    delay(50);
}
