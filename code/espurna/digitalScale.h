#ifndef __DIGITAL_SCALE__
#define __DIGITAL_SCALE__

#include "espurna.h"

#include "mqtt.h"
#include "ntp.h"
#include "ArduinoJson.h"
#include "SoftwareSerial.h"
#include "button.h"
#include "button_config.h"
#include "SICSCommand.h"
#include "ws.h"
#include "i2cExtender.h"
#include "LoRa_Transmission.h"

#define NUMER_OF_REQUEST            1
#define START_WEIGHT_POSITION       5
#define END_WEIGHT_POSITION         16
#define ST_START_WEIGHT_POSITION    4
#define ST_END_WEIGHT_POSITION      15
#define BTN_RECORD_WEIGHT           0
#define ACTION_PRESSED              1
#define ACTION_CLICK                2
#define BAUND_RATE_MCU              9600
#define MAX_MESSAGE                 20
#define START_ID_POSITION           7
#define START_TYPE_POSITION         7
#define MAX_JSON_DATA               500
#define INVALID_WEIGHT              -1.0
#define START_WEIGHT_POSITION_ST    0
#define END_WEIGHT_POSITION_ST      3
#define INVALID_TIME                "0000-00-00 00:00:00"
#define TOPIC_WS_PONDID             "backPondID"
#define TOPIC_WS_TYPE               "backType"

#ifndef __STD_RETURN_TYPE__
#define __STD_RETURN_TYPE__
typedef int8_t STD_RETURN_TYPE;
#define E_OK       0
#define E_NOT_OK   1
#define E_PENDING  -1
#endif

typedef struct mqtt_packet_data {
    String ID_str = "";
    int number_n = 1;
    String weight_str = "0.0";
    String pondID_str = "";
    String type_str = "";
} t_mqtt_packet_data;

extern t_mqtt_packet_data g_mqtt_packet_data;
extern SoftwareSerial SUART;
extern std::vector<button_t> _buttons;

void DisplayMessage(String message);

void icsSetup();

#endif