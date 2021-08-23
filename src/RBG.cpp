#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "config.h"
#include "common.h"

boolean ledStates[3];
int leds[3] = {RED_LED_PIN,BLUE_LED_PIN,GREEN_LED_PIN};


void RBGAction(eLed led, eAction action) {
    switch (action)
    {
    case LED_ON:
        digitalWrite(leds[led],HIGH);
        ledStates[led] = true;
        break;
    case LED_OFF:
        digitalWrite(leds[led],LOW);
        ledStates[led] = false;
        break;
    case LED_TOGGLE:
        digitalWrite(leds[led],ledStates[led] ?LOW : HIGH);
        ledStates[led] = !ledStates[led];
        break;
    
    default:
        break;
    }
}

void RBGInit() {
    pinMode(RED_LED_PIN,OUTPUT);
    pinMode(GREEN_LED_PIN,OUTPUT);
    pinMode(BLUE_LED_PIN,OUTPUT);
    pinMode(ONBOARD_LED,OUTPUT);
#if 1
    RBGAction(RED_LED,LED_ON);
    delay(100);
//    vTaskDelay(100/portTICK_PERIOD_MS);  // no vTskDelay before scheduler started
    RBGAction(RED_LED,LED_OFF);
    RBGAction(BLUE_LED,LED_ON);
    delay(100);
//    vTaskDelay(100/portTICK_PERIOD_MS);
    RBGAction(BLUE_LED,LED_OFF);
    RBGAction(GREEN_LED,LED_ON);
    delay(100);
//    vTaskDelay(100/portTICK_PERIOD_MS);
    RBGAction(GREEN_LED,LED_OFF);
#endif
}