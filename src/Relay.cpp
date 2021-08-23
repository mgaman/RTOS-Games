#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "config.h"
#include "common.h"

#define PULSE_WIDTH ((80/portTICK_PERIOD_MS)+1)  // msec to tick

void RelaySet(int in1, int in2);
void TapOpen();
void TapClose();

void RelayInit() {
// initialize relay pins and turn power off to output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  RelaySet(LOW,HIGH);
//  Serial.print("Relay pulse set to ");
//  Serial.println(PULSE_WIDTH);
#if 0    // can't do this before scheduler starts
    TapOpen();
    TapClose();
#endif
}

void RelaySet(int in1, int in2)
{
  // sets power on to other side of relay
  digitalWrite(IN1, in1);
  digitalWrite(IN2, in2);
}

void TapOpen()
{
  RelaySet(HIGH,HIGH);
  vTaskDelay(PULSE_WIDTH);
  RelaySet(HIGH,LOW);
}
void TapClose()
{
  RelaySet(LOW,LOW);
  vTaskDelay(PULSE_WIDTH);
  RelaySet(LOW,HIGH);
}
