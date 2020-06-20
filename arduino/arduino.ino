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
#define configUSE_PREEMPTION 1

bool host_ready = false;

QueueHandle_t to_send_q;
QueueHandle_t lcd_q; // Kolejka do której się macie podpiąć
SemaphoreHandle_t binarysem;
TaskHandle_t rcvHandle = NULL;
TaskHandle_t sndHandle = NULL;
TaskHandle_t lcdHandle = NULL; // Wasz task
 
void rcvTaskFunction(void * pvParameters)
{
    int ac_output [] = {4,6,8,10};
    AudioController audio_controller(2, ac_output);
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    for( ;; )
    {
        byte start = 0;
        do {
          if(Serial.available() > 0){
            start = Serial.read();
          }
        } while(start != 254);

        byte header [4];
        int read_for_header = 0;
        do {
           if(Serial.available() > 0){
            header[read_for_header] = Serial.read();
            read_for_header++;
          }
        } while(read_for_header < 4);

        /*for(int i = 0; i < 4; i++){
          Serial.write(header[i]);
        }*/
    
        byte no_of_bytes;
        int read_for_no = 0;
        if(false){
            do {
              if(Serial.available() > 0){
                no_of_bytes = Serial.read();
                read_for_no++;
              }
            } while(read_for_no != 1);
        }else{
            no_of_bytes = 5;
        }     
        //debug(no_of_bytes); 

        byte * data = new byte[no_of_bytes];
        int read_for_data = 0;
        do {
           if(Serial.available() > 0){
              data[read_for_data] = Serial.read();
              read_for_data++;
          }
        } while(read_for_data < no_of_bytes);

        /*for(int i = 0; i < no_of_bytes; i++){
          Serial.write(data[i]);
        }*/
        
        byte control_sum;
        int read_for_sum = 0;
        do {
          if(Serial.available() > 0){
            control_sum = Serial.read();
            read_for_sum++;
          }
        } while(read_for_sum != 1);
        //Serial.write(control_sum);\
        
        Message received = codec_decode(header, data, no_of_bytes);
        switch(received.header){
          case CSND:
          case TSND:
          case RSND:
            while(true){
                int result = xQueueSend( lcd_q,( void * ) &received,( TickType_t ) 1 );
                if( result == pdTRUE){
                  break;
                }else if( result == errQUEUE_FULL ){
                  xQueueReceive(lcd_q,NULL,(TickType_t) 1);
                }
                vTaskDelay( 1 );
            }
            break;
          case ASND:
            digitalWrite(LED_BUILTIN, HIGH);
            audio_controller.setOutputs(received.numeric);
            break;
        }

         xSemaphoreTake(binarysem,( TickType_t )0 );
         if(received.header == HRDY && !host_ready){ //!host_ready){
            host_ready = true;
            //digitalWrite(LED_BUILTIN, HIGH);
            Message erdy;
            erdy.header = ERDY;
            erdy.numeric = 420;
            while(true){
              if(xQueueSend( to_send_q,( void * ) &erdy,( TickType_t ) 1 ) != pdTRUE){
                break;
              }
              vTaskDelay( 1 );
            }
            xSemaphoreGive(binarysem);
         }
        
        delete [] data;
        vTaskDelay( 1 );
        //vTaskDelay(3);  // one tick delay (15ms) in between reads for stability
       }
    vTaskDelete( NULL );
}

void send_message(Message & mes){
  size_t datasize;
  byte * encoded= codec_encode(mes, datasize);
  for(int i = 0; i < datasize; i++){
        Serial.write(encoded[i]);
  }
  delete [] encoded;
  vTaskDelay( 1 );
}

void sndTaskFunction(void * pvParameters)
{
  configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
  //vTaskSuspend( NULL );
  for(;;){
    //Serial.write(1);
    //if( xSemaphoreTake( binarysem, ( TickType_t ) 10 ) == pdTRUE )
    xSemaphoreTake(binarysem,( TickType_t )0 );
    if(host_ready)
    {
        Message to_send;
        if(xQueueReceive(to_send_q,&to_send,(TickType_t) 1)== pdPASS){
          send_message(to_send);
          vTaskDelay( 1 );
        }
        /*unsigned long val = analogRead(2);
        Message audiomsg;
        audiomsg.header = ASET;
        audiomsg.numeric = val;
        send_message(audiomsg); */
        vTaskDelay( 2 );
        xSemaphoreGive(binarysem);
    }
  }
}

/*void {wasza funkcja}(void * pvParameters){
  configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
  for(;;){
    Serial.println("eee");
  }
}*/

void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    binarysem = xSemaphoreCreateBinary();
    xSemaphoreGive(binarysem);
    //xSemaphoreTake(binarysem,( TickType_t )10 );
    /*if( binarysem == NULL )
    {
       //Serial.write(202);
       //digitalWrite(LED_BUILTIN, HIGH); 
    }*/
    to_send_q = xQueueCreate( 10, sizeof( struct Message ) );
    //////////////////////
    //Do tej kolejki się podpinacie elegancko
    lcd_q = xQueueCreate( 10, sizeof( struct Message ) );
    
    xTaskCreate(rcvTaskFunction,"rcv_task", 400, ( void * ) 1, 1, &rcvHandle);
    xTaskCreate(sndTaskFunction,"snd_task", 256, ( void * ) 1, 1, &sndHandle);

    //////////////////////
    //Odkomentujcie, wstwcie swoją funkcję
    //Gdyby arduino się zacinało i zaczynało mrygać diodą L powoli = stack overflow -> tam gdzie jest 256 zwiększcie rozmair stosu dla taska ale nie powinno się tak dziać
    //Gdyby mrygało diodą L szybko to jest heap overflow i się upewnijcie że kasujecie wszystko co dynamicznie alokowane
    //Zmieniłem w Message string na stary dobry char * bo stringi na arduino są fe
    //xTaskCreate({wasza funkcja},"lcd_task", 256, ( void * ) 1, 1, &lcdHandle);
    /////////////////////
}

void loop()
{
  //
}
