#include <Arduino_FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
/*#include <FreeRTOSConfig.h>
#define configSUPPORT_STATIC_ALLOCATION 1*/

#include "AudioController.h"
#include "message.h"
#include "codec.h"

#define SERIAL_RX_BUFFER_SIZE 256
#define configUSE_TIME_SLICING 1
#define configUSE_PREEMPTION 0

AudioController audio_controller;

SemaphoreHandle_t xSerialSemaphore;
int potPin = 2;    // select the input pin for the potentiometer
int ledPin = 12;   // select the pin for the LED
int t1 = 0;
int aset_acc = 0;
//bool host_ready = false;

//QueueHandle_t to_send_q;

int read_byte(byte & buf){
  //if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
    if (Serial.available() > 0){
        byte incoming = Serial.read();
        buf = incoming;
        //xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
        return 1;
    }else{
      //vTaskDelay(1);
      //xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
      buf = 'e';
      return 0;
    }
  //}
}

void debug(byte b){
  if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
    Serial.write(b);
    xSemaphoreGive( xSerialSemaphore );
  }
}

void rcvTaskFunction(void * pvParameters)
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    //Serial.println("siemka");
    for( ;; )
    {
        int bytes_read; 
        byte messageStart;
        do {
          bytes_read = read_byte(messageStart);
          //Serial.println(int(bytes_read));
          if(bytes_read > 0){
            debug(messageStart);
          }
        } while(bytes_read != 1 && messageStart != 254);
    
        // read header
        int read_for_header = 0;
        byte header [4];
        do {
           byte rb;
           bytes_read = read_byte(rb);
           header[read_for_header] = rb;
           read_for_header += bytes_read;
        } while(read_for_header < 4);
    
        byte no_of_bytes;
        if(false){
            do {
              int bytes_read = read_byte(no_of_bytes);
            } while(bytes_read != 1);
        }else{
            no_of_bytes = 5;
        }     
        //debug(no_of_bytes); 

        byte * data = new byte[no_of_bytes];
        int read_for_data = 0;
        do {
           byte rb;
           bytes_read = read_byte(rb);
           data[read_for_data] = rb;
           read_for_data += bytes_read;
           if(bytes_read > 0){
              //debug(rb);
           }
        } while(read_for_data < no_of_bytes);

        //debug(header[0]);
        /*for(int i = 0; i < 4; i++){
          debug(header[i]);
        }*/
        
        byte control_sum;
        do {
            bytes_read = read_byte(control_sum);
        } while(bytes_read != 1);
        
        Message test = codec_decode(header, data, no_of_bytes);

         if(test.header == HRDY && true){ //!host_ready){
            Message erdy(ERDY, 420);
            //xQueueSend( to_send_q,( void * ) &erdy,( TickType_t ) 10 );
        //if(memcmp(header,"HRDY",4) == 0 && !host_ready){
          //digitalWrite(LED_BUILTIN, LOW);
          //host_ready = true;
        }
        
        delete [] data;
       }
    //vTaskDelete( NULL );
}

void sndTaskFunction(void * pvParameters)
{
  //Serial.write(244);
  configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
  if(true){
    send_message();
  }
  vTaskDelay(1);
}

//#define STACK_SIZE 200
//StaticTask_t xTaskBuffer;
//StackType_t xStack[ STACK_SIZE ];

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.

void setup()
{
    // inicjalizacja wejść
    //sc.initInput(2); // ustawienie pinu dla potencjometru


    // inicjalizacja wyjść
    //int outputs[] = {4, 5, 6, 7};
    //sc.initOutputs(outputs); // piny dla diód
    //audio_controller =AudioController(2,outputs);
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
    }   

    if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
    {
      xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
      if ( ( xSerialSemaphore ) != NULL )
        xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
    }
    //Create a queue of messages
    //to_send_q = xQueueCreate( 10, sizeof( Message ) );
    
    //BaseType_t xReturned;
    TaskHandle_t rcvHandle = NULL;
    TaskHandle_t sndHandle = NULL;
    //TaskHandle_t xHandle = NULL;

    /* Create the task without using any dynamic memory allocation. */
    xTaskCreate(rcvTaskFunction,"rcv_task", 256, ( void * ) 1, 1, &rcvHandle);
    xTaskCreate(sndTaskFunction,"snd_task", 256, ( void * ) 1, 1, &sndHandle);

    /*if( ( to_send_q == NULL ))
     {
     }*/
}

void loop()
{

}

void send_message(){
  unsigned long val = analogRead(2);
  Message mes(ASET,val);
  size_t datasize;
  byte * encoded= codec_encode(mes, datasize);
  for(int i = 0; i < datasize; i++){
      //if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
        Serial.write(encoded[i]);
        //xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
      //}
  }
  /*Serial.print(' ');
  for(int i = 0; i < datasize; i++){
    Serial.print(int(encoded[i]));
    Serial.print(' ');
  }
  Serial.print('\n');*/
  delete [] encoded;
}

void comment()
{
  //Serial.println("com");
  int t2 = millis();
    int delta = t2 - t1;
               // stop the program for some time*/  
    t1 = t2;
    delay(50);
  /////////////////////
    
  
    delay(20);
}
