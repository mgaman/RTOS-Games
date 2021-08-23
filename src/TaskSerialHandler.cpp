#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "queue.h"
#include "config.h"
#include "common.h"

/*
    Menu of commands (each a single character)
    h print help
    r red led on
    R red led off
    o open tap
    O close tap
*/

const char *host = "AT+CIPSTART=\"TCP\",\"" DEFAULT_HTTP_SERVER "\",80";
const char *startTx = "AT+CIPSEND";
const char *get = "GET /webdata/WC/RawData.php?AC=report&imei=1234&UT=123&m0=1&m1=2&epoch=123456&tap=2&ss=17 HTTP/1.1";
//const char *get = "GET /webdata/WC/last34.php HTTP/1.1";
//const char *get = "GET / HTTP/1.1";
char linebuf[150];
extern int biggest;

void TaskSerialHandler( void *pvParameters ) {
    while (true) {
#if 0
        // simple copy from serial to serial1
        if (Serial.available()) {
            //printHWM("Serial0");
            Serial1.write(Serial.read());
        }
#else
        if (Serial.available()) {
            //printHWM("Serial0");
            switch (Serial.read()) {
                case 'b':
                    Serial.print(F("Biggest "));
                    Serial.println(biggest);
                    break;
                case 'g':
                    Serial.println(get);
                    Serial1.println(get);
                    Serial1.println();
                    Serial1.write((byte)0x1a);
                    break;
                case 'h':
                    Serial.println(host);
                    Serial1.println(host);
                    break;
                case 'H':
                    Serial.println(startTx);
                    Serial1.println(startTx);
                    break;
                case 'o':
                    TapOpen();
                    break;
                case 'O':
                    TapClose();
                    break;
                case 'r':
                    RBGAction(RED_LED,LED_ON);
                    break;
                case 'R':
                    RBGAction(RED_LED,LED_OFF);
                    break;
                case 's':
                    Serial1.println("AT+CIPSTATUS");
                    break;
                case 'S':
                    Serial1.println("AT+CIPCLOSE");
                    break;
                default:
                    Serial.println("Commands");
                    Serial.println("g send get");
                    Serial.println("h connect to host");
                    Serial.println("H prepare to send");
                    Serial.println("o open tap");
                    Serial.println("O close tap");
                    Serial.println("r red led on");
                    Serial.println("R red led off");
                    Serial.println("s cipstatus");
                    Serial.println("S cipclose");
                    break;
            }
        }
#endif
    }
}
