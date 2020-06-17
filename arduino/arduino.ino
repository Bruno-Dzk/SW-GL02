#include "AudioController.h"
#include "message.h"
#include "codec.h"

AudioController audio_controller;

int potPin = 2;    // select the input pin for the potentiometer
int ledPin = 13;   // select the pin for the LED
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
  unsigned long val = analogRead(2);
  Message mes(HSND,"EE");
  size_t datasize;
  byte * encoded= codec_encode(mes, datasize);
  /*for(int i = 0; i < datasize; i++){
      Serial.write(encoded[i]);
  }*/
  Serial.print(val);
  Serial.print(' ');
  for(int i = 0; i < datasize; i++){
    Serial.print(int(encoded[i]));
    Serial.print(' ');
  }
  Serial.print('\n');
  delete [] encoded;
}

unsigned long counter = 0;
bool host_ready = false;

void loop()
{
  if (true) {
    // read the incoming byte:
    byte incomingByte = Serial.read();

    // say what you got:
    Serial.println(incomingByte, DEC);
  }
  /*byte messageStart;
    do {
      if(Serial.available() > 0){
        messageStart = Serial.read();
        Serial.println(int(messageStart));
      }
      //if(Serial.available() > 0){
        //Serial.println(int(messageStart)); 
      //}
    } while(messageStart != 254);

    // read header
    int read_for_header = 0;
    byte header [4];
    do {
      if(Serial.available() > 0){
        byte read_byte = Serial.read();
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
        int read_for_no = 0;
        do {
          if(Serial.available() > 0){
            byte read_byte = Serial.read();
            no_of_bytes = read_byte;
            read_for_no += 1;
          }
        } while(read_for_no < 1);
    }else{
        no_of_bytes = 5;
    }       

    byte * data = new byte[no_of_bytes];
    int read_for_data = 0;
    do {
      if(Serial.available() > 0){
        byte read_byte = Serial.read();
        data[read_for_data] = read_byte;
        read_for_data += 1;
      }
    } while(read_for_data < no_of_bytes);*/
    

    ////////////////
    /*(for(int i = 0; i < no_of_bytes; i++){
      Serial.print(int(data[i]));
      Serial.print(' ');
    }
    Serial.print('\n');*/
    /////////////
    
    /*byte control_sum;
    int read_for_sum = 0;
        do {
          if(Serial.available() > 0){
            byte read_byte = Serial.read();
            control_sum = read_byte;
            read_for_sum += 1;
          }
        } while(read_for_sum < 1);
    Message test = codec_decode(header, data, no_of_bytes);
    /*Serial.println(test.text);
    if(test.header == HRDY && !host_ready){
      host_ready = true;
    }
    
    delete [] data;
    if(host_ready){
      send_message();
    }*/
    delay(500);
}

void comment()
{
  int t2 = millis();
    int delta = t2 - t1;
               // stop the program for some time*/  
    t1 = t2;
    delay(50);
  /////////////////////
    
  
    delay(20);
}
