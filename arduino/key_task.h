#pragma once
#include <Keypad.h>
#include <Arduino_FreeRTOS.h>

const byte ROWS = 2;
const byte COLS = 4;
byte keytable[ROWS][COLS] = {
  {1,2,3,4},
  {5,6,7,8}
};
const byte rowPins[ROWS] = {30, 32};
const byte colPins[COLS] = {28, 26, 24, 22};

void keyTaskFunction(void * pvParameters)
{
  QueueHandle_t to_send_q = ( QueueHandle_t ) pvParameters;
  Keypad kpad = Keypad( makeKeymap(keytable), rowPins, colPins, ROWS, COLS); 
  for(;;){
    unsigned long pressed = kpad.waitForKey();
    Message keypress_msg;
    keypress_msg.header = KEYP;
    keypress_msg.numeric = pressed - 1;
    while(true){
      if(xQueueSend( to_send_q,( void * ) &keypress_msg,( TickType_t ) 1 ) == pdTRUE){
        break;
      }
      vTaskDelay( 1 );
    }
  }
}
