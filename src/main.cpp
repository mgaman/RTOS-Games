#include <Arduino.h>
#include <HardwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <stream_buffer.h> 
//#define MAIN_FILE
#include "config.h"
#include <math.h>

#define MAIN_MODULE
#include "common.h"

// In PlatformIO MUST use prototypes for functions

void printHWM(char *);

void setup() {
    boolean tasksCreated = true;    // start optimistic
    // All pinModes etc handles by individual tasks
    Serial.begin(115200); 
    Serial1.begin(MODEM_BAUDRATE); 
//    RBGInit();
//    RelayInit();
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
    }
    // create global data
    hShortMessageQueue = xQueueCreate(SHORT_MESSAGE_QUEUE_SIZE,SHORT_MESSAGE_LENGTH);
    hRawMessageQueue = xQueueCreate(RAW_MESSAGE_QUEUE_SIZE,sizeof(sRawMessage));
    unsolicitedState = true; 
    u1linemode = true; 
    
    if (hShortMessageQueue == NULL || hRawMessageQueue == NULL) {
        Serial.println("Queues not created");
        while (true) {}
    }
    tasksCreated &= (pdPASS == xTaskCreate(
        TaskStartup
        ,  "Startup" 
        ,  100  // This stack size was checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,  NULL ));
    tasksCreated &= (pdPASS == xTaskCreate(
        TaskSerial1Handler
        ,  "S1H"
        ,  170  // This stack size was checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,  NULL ));
    // check that all OK
    if (tasksCreated == pdPASS) 
        vTaskStartScheduler();
    else {
        Serial.println("Tasks not created, hanging");
        while (true) {}
    }
}

void loop() {
  // never any code here
}
