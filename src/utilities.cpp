#include <Arduino.h>
#include <HardwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stream_buffer.h> 
#include "config.h"
#include <math.h>

#include "common.h"

void printHWM(char * tn) {
  Serial.print(tn); Serial.print(' ');
  UBaseType_t HWM;    // stack high water mark
  HWM = uxTaskGetStackHighWaterMark(NULL);  // self
  Serial.println(HWM);
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ){
    RBGAction(GREEN_LED,LED_ON);
    while(1){}
}

void vApplicationMallocFailedHook( void ) {
    RBGAction(BLUE_LED,LED_ON);
    while (true) {}
}