#include <Arduino.h>
#include <HardwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stream_buffer.h> 
#include "config.h"
#include <math.h>

#include "common.h"

void TaskUnsolicited(void *pvParameters);
int biggest = -1;

char S1linebuf[SHORT_MESSAGE_LENGTH+10];
char rawBuf[LARGEST_EXPECTED_MESSAGE];   // size of UART ISR buffer
void TaskSerial1Handler( void *pvParameters ) {
    int hiWater = 0,loWater = 0;
    int lineIndex = 0;
    int len;
    sRawMessage sRM;

    while (true) {
        len = Serial1.available();
        if (len > 0) {
            if (len > biggest)
                biggest = len;
            loWater = hiWater = 0;
            //printHWM("Serial1 Handler");
            int inread = Serial1.readBytes(&rawBuf[hiWater],len);
            if (inread < len)
                Serial.println("partial");
            hiWater = inread;
            if (u1linemode) {
                // copy unread data into S1linebuf
                // when complete line detected, send it via message queue, 
                // continue until all data transferred
                // reset hi/lo waters
                while (loWater < hiWater) {
                    // ignore nul characters
                    if (rawBuf[loWater] == 0) {
                        loWater++;
                    }
                    else {
                        char c = rawBuf[loWater++];
                        //Serial.print(c);
                        S1linebuf[lineIndex++] = c;
                        if (lineIndex >= SHORT_MESSAGE_LENGTH) {
                            Serial.println("Message length high");
                        }
                        if (c == 0x0a) {
                            S1linebuf[lineIndex] = 0;
                            lineIndex = 0;
                            //printHWM("Serial1 Handler");
                            if (xQueueSend(hShortMessageQueue,S1linebuf,1) != pdPASS) {
                                Serial.println("shortmessage fail");
//                                vTaskDelete(NULL);
                            }
                        }
                    }
                }
            }
            else {
                // transfer all data to a raw message
                sRM.length = hiWater;
                memcpy(sRM.rawData,rawBuf,hiWater);
                xQueueSend(hRawMessageQueue,(void *)&sRM,1);
            }
        }
        else
            taskYIELD();
    }
}
