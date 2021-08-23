// Global Data
#ifdef MAIN_MODULE
boolean unsolicitedState = true;    // UART1 input is unsolicited, unsolicited task must ignore data
boolean u1linemode = true;          // UART1 input to be aggregated as lines, else raw as-is

QueueHandle_t hShortMessageQueue, hRawMessageQueue;
#else
extern boolean unsolicitedState;    // UART1 input is unsolicited, unsolicited task must ignore data
extern boolean u1linemode;          // UART1 input to be aggregated as lines, else raw as-is
extern QueueHandle_t hShortMessageQueue, hRawMessageQueue;
#endif

struct sRawMessage {
    int length;
    byte rawData[RAW_MESSAGE_LENGTH];
};
// prototypes
void TaskSerial1Handler(void *pvParameters);
void TaskSerialHandler( void *pvParameters );
void TaskStartup(void *pvParameters);
void TaskUnsolicited(void *pvParameters);
void printHWM(char * tn);
void TapOpen();
void TapClose();
void RelayInit();
// leds
enum eLed {RED_LED,BLUE_LED,GREEN_LED};
enum eAction {LED_ON,LED_OFF,LED_TOGGLE};
void RBGInit();     // RBG led stuff
void RBGAction(eLed, eAction);

