#include <Arduino_FreeRTOS.h>
#include <queue.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

enum MsgType{
  HRDY,
  ERDY,
  ASET,
  ASND,
  CSND,
  RSND,
  TSND,
  KEYP,
  VSND,
  HGET,
  HSND
};

struct Message
{
  MsgType header;
  unsigned int numeric;
  String text;
//  Message(MsgType, unsigned int);
//  Message(MsgType, String);
};


LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7,3,POSITIVE);
String inData;
QueueHandle_t xQueue1;
Message mes={HSND,5,"cos"};




void setup() {
    Serial.begin(9600);
    lcd.begin(16,3);
    lcd.backlight();

    xQueue1 = xQueueCreate( 10, sizeof( struct Message * )  );

   if( xQueue1 == NULL )
    {
        /* Queue was not created and must not be used. */
        lcd.println("Queue can not be created");
    }

    xTaskCreate(TaskDisplay,"Display_task", 128, NULL, 1, NULL);
    xTaskCreate(TaskLDR, "LDR_task", 128, NULL, 1, NULL);
    vTaskStartScheduler();
}

void loop() {

}
void TaskLDR(void * pvParameters) {

  while(1) {

    xQueueSend(xQueue1, &mes, portMAX_DELAY);
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}

void TaskDisplay(void * pvParameters) {
  
  while(1) {
    //lcd.println("TaskDisplay");
    if (xQueueReceive(xQueue1, &mes, portMAX_DELAY) == pdPASS) {
      
      if (mes.header == 6){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CPU Temp.: ");
      lcd.setCursor(11, 0);
      lcd.print(mes.numeric);
      lcd.setCursor(14, 0);
      lcd.print("C");
      }
      else if (mes.header== 4){
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("CPU Usage: ");
      lcd.setCursor(11, 1);
      lcd.print(mes.numeric);
      lcd.setCursor(14, 1);
      lcd.print("%");
      }
      else if (mes.header==5){
      lcd.clear();
      lcd.setCursor(0, 2);
      lcd.print("GPU Usage: ");
      lcd.setCursor(11, 2);
      lcd.print(mes.numeric);
      lcd.setCursor(14, 2);
      lcd.print("%");
      }
      else if(mes.header==10){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(mes.text);
       
        
      }
    }
  }
}
