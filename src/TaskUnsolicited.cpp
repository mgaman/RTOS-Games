#include <Arduino.h>
#include <HardwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stream_buffer.h> 
#include "config.h"
#include <math.h>

#include "common.h"

const char *cgregstates[] = {
    "not registered",
    "registered, home",
    "not registered",
    "registration denied",
    "unknown",
    "registered, roaming"
};
void TaskUnsolicited( void *pvParameters ) {
    // Read the next message, if available on queue.
    //
    char linebuf[SHORT_MESSAGE_LENGTH];
    Serial.println("unsolicited started");
    boolean nextLineSMS = false;
    boolean ipaddressprinted = false;
//    Serial1.println("AT+CIFSR");  // Get our ip address
    while (true) {
        if(unsolicitedState && (xQueueReceive(hShortMessageQueue, linebuf, 0) == pdPASS))
        {
            //printHWM("Unsolicited");
            //Serial.print("<<<");
            Serial.print(linebuf);
            // check for known responses
            if (strncmp("+CLIP",linebuf,5) == 0)    // caller id
            {
                // isolate number
                char *start = strchr(linebuf,'"');
                char *end = strchr(++start,'"');
                *end = 0;
                Serial.print("Incoming call from ");
                Serial.println(start);
            }
            else if (strncmp("+CMT:",linebuf,5) == 0) { // incoming SMS
                // isolate number
                char *start = strchr(linebuf,'"');
                char *end = strchr(++start,'"');
                *end = 0;
                Serial.print("Incoming SMS from ");
                Serial.print(start);
                Serial.print(" ");
                nextLineSMS = true;
            }
            else if (nextLineSMS) {
                Serial.print(linebuf);
                nextLineSMS = false;
            }
            else if (strncmp("+CIEV:",linebuf,6) == 0) {
                char *start = linebuf+7;
//                char *end = strchr(start, ',');
//                *end = 0;
                Serial.print("Event ");
                Serial.println(start);
                // analyse event
                if (strncmp("\"CALL\"",start,6)==0) {
                    start = strchr(start,',');
                    int value = atoi(++start);
                    Serial.print("Call ");
                    Serial.println( value == 0 ? "disconnected" : "connected");
                }
            }
            else if (strncmp("+CGREG:",linebuf,7) == 0) {
                char *start = strchr(linebuf,' ');
                int value = atoi(++start);
                Serial.print("Network registration is ");
                Serial.println(cgregstates[value]);
                if (!ipaddressprinted) {
                    Serial1.println("AT+CIFSR");  // Get our ip address
                    ipaddressprinted = true;
                }
            }
            else if (strncmp("+HTTPACTION",linebuf,11) == 0)
                Serial1.println(F("AT+HTTPREAD"));
        }
    }
}
