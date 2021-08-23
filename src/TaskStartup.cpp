#include <Arduino.h>
#include <HardwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stream_buffer.h> 
#include "config.h"
#include <math.h>

#include "common.h"

const char *atcommands[] = {
    "ATE1",
    "AT+CLIP=1", // enable callerid same A6/SIM900
    "AT+CMGF=1",  // SMS text mode  same
#ifdef SIM900
    "AT+SAPBR=3,1,\"APN\",\"" DEFAULT_APN "\"", 
    "AT+SAPBR=2,1",  // GET IP ADDRESS
    "AT+SAPBR=4,1",
    "AT+SAPBR=1,1",  // OPEN
    "AT+SAPBR=2,1",  // GET IP ADDRESS
    "AT+HTTPINIT",
    "AT+HTTPPARA=\"CID\",1",
    "AT+HTTPPARA=\"URL\",\"henrylineage.com\"",
    "AT+HTTPACTION=0"
#else
    "ATQ1",  // suppress DCE responses same
    "AT+CGDCONT=1,\"IP\",\"" DEFAULT_APN "\"",  // setup pdp parameters  same
    "AT+CGATT=1", // attach to packet domain service
    "AT+CGACT=1,1"   // activate PDP
#endif
};
#define LAST_CASE (sizeof(atcommands)/sizeof(const char *)-1)
 
void hwreset() {
#ifdef A6
    digitalWrite(MODEM_RESET_TRIGGER,LOW);
    digitalWrite(MODEM_RESET_TRIGGER,HIGH);
    //delay(50);
    vTaskDelay(50/portTICK_PERIOD_MS);
    digitalWrite(MODEM_RESET_TRIGGER,LOW);
#elif defined(SIM900)
    digitalWrite(MODEM_RESET_TRIGGER,LOW);
//	delay(1000);
    vTaskDelay(1000/portTICK_PERIOD_MS);
	digitalWrite(MODEM_RESET_TRIGGER,HIGH);
//	delay(2500);
    vTaskDelay(2550/portTICK_PERIOD_MS);
	digitalWrite(MODEM_RESET_TRIGGER,LOW);
//	delay(3500);
    vTaskDelay(3550/portTICK_PERIOD_MS);
#endif
}
char msgBuf[SHORT_MESSAGE_LENGTH];
void TaskStartup( void *pvParameters ) {
    Serial.println("Startup start");
    // do hardware reset of modem
    pinMode(MODEM_RESET_TRIGGER,OUTPUT);
    hwreset();
    unsolicitedState = false;
    int stage = 0;
    int atindex = 0;
    // now start waiting for specific messages
    
    while (true) {
#if 0
        if (Serial1.available()) {
            char c = Serial1.read();
            Serial.print(c);
            if (c == 's')
                printHWM("Start");
        }
#endif
        if ((!unsolicitedState) && xQueueReceive(hShortMessageQueue, msgBuf, 0) == pdPASS) {
//            Serial.print("@@@");
//            printHWM("Start");
            Serial.print(msgBuf);
            switch (stage) {
                case 0:  // waiting for CREG   
                    if (strncmp("NORMAL",msgBuf,6) == 0)
                        hwreset();
                    if (strncmp("+CREG: 1",msgBuf,8) == 0) {
                        stage++;
                        Serial1.println(atcommands[atindex++]);  // no echo
                    }
                    break;
                case 1 ... LAST_CASE:
                    //Serial.print(F("Stage ")); Serial.println(stage);
                    if (strncmp("OK",msgBuf,2) == 0) {
                            vTaskDelay(5000/portTICK_PERIOD_MS);
                            Serial1.println(atcommands[atindex++]);  // rest of commands
                            stage++;
                        }
                    break;
                default:
                    // got to end of list
                    if (strncmp("OK",msgBuf,2) == 0) {
                        Serial.println("Startup finished");
                        BaseType_t tasksCreated; 
#if 1
                        taskENTER_CRITICAL();
                        unsolicitedState = true;
                        taskEXIT_CRITICAL();
                        tasksCreated = xTaskCreate(
                            TaskUnsolicited
                            ,  "Unsol"   // A name just for humans
                            ,  220  // This stack size was checked & adjusted by reading the Stack Highwater
                            ,  NULL
                            ,  2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
                            ,  NULL );
#endif
                        tasksCreated &= xTaskCreate(
                            TaskSerialHandler
                            ,  "S0H"   // A name just for humans
                            ,  110 // This stack size was checked & adjusted by reading the Stack Highwater
                            ,  NULL
                            ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
                            ,  NULL );
                        if (tasksCreated != pdPASS)
                            Serial.println("2nd tasks not created");
                        vTaskDelete(NULL);
                    }
                    break;
            }
        }
    }
}
