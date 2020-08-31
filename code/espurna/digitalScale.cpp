#include "digitalScale.h"
#include "LoRa_Transmission.h"

// MQTT packet data
t_mqtt_packet_data g_mqtt_packet_data;
// declare for global variable
SoftwareSerial SUART( 9, 10); // TX: D1 & RX: D2

void _settingCOM()
{
    // baurate : 9600
    UART_PORT.begin(BAUND_RATE_MCU);
    DEBUG_MSG_P(PSTR("Setting baudrate of UART_PORT to 9600.\n"));
}

void _settingBuzzer()
{
    // Set buzzer pin to ouput
    i2cuart.pinMode(1, OUTPUT);
    // Set buzzer to low at begin
    i2cuart.digitalWrite(BUZZER_PIN, LOW);
}
String _sendCMD(String command)
{
    String responce = RESPONCE_ERROR;
    DEBUG_MSG_P(PSTR("[MCU] Command request: %s\n"), command.c_str());
    UART_PORT.println(command);
    //Read responce from balance
    while(UART_PORT.available() >0)
    {
        responce = UART_PORT.readString();
        DEBUG_MSG_P(PSTR("[ICS449] Responce: %s\n\n"), responce.c_str());
    }
    return responce;
}

void DisplayMessage(String message)
{
  String cmd = "D  ", responce = RESPONCE_ERROR;
  DEBUG_MSG_P(PSTR("%s\n"), message.c_str());
  // Write text into balance display.
  // D_[space]_"message" + \r\n
  #ifdef BALANCE_DEBUG
    cmd = "D \"" + message + "\"";
    (void)responceTextDisplay(_sendCMD(cmd));
  #else
  if(message.length() < MAX_MESSAGE)
  {
    cmd = "D \"" + message + "\"";
    (void)_sendCMD(cmd);
  } else
  {
    // cut the text and display for each parts
    String sub_message = message.substring(MAX_MESSAGE);
    message = message.substring(0, MAX_MESSAGE);
    cmd = "D \"" + message + "\"";
    (void)_sendCMD(cmd);
    delay(400);
    cmd = "D \"" + sub_message + "\"";
    (void)_sendCMD(cmd);
  }
  #endif
}

void _startInformation()
{
    String responce = RESPONCE_ERROR, message = "";
    // Display introduction of the software to balance
    DisplayMessage("Welcome");
    delay(50);
    DisplayMessage("Getting started");
    delay(50);

    #if 0
    // Get balance ID and display to balance
    responce = _sendCMD(GET_BALANCE_ID);
    // Get balance ID from the responce
    responce = responce.substring(START_ID_POSITION);
    // Display balance ID into balance
    message = "Balance ID: " + responce;
    DisplayMessage(message);

    // Get balance type and display to balance
    responce = _sendCMD(GET_BALANCE_TYPE);
    // Get balance type from the responce
    responce = responce.substring(START_TYPE_POSITION);
    // Display balance type into balance
    message = "Balance type: " + responce;
    DisplayMessage(message);
    #endif

    DisplayMessage("Ready for use!");
}

float _getWeightValue()
{
    DisplayMessage("Please wait . . .");
    float_t sum_weight = 0.0;
    // command to send the current net weight value: SI
    String weight_str = "0.0";
    String responce = RESPONCE_ERROR;
    uint8_t numberOfSuccess_n = 0;
    // get the average value from weight vaue which is received from scale balance
    for(uint8_t i=0; i< NUMER_OF_REQUEST; i++)
    {
        // Request send scale value and get the responce
        responce = _sendCMD(GET_SCALE_VALUE);

        if(responceWeightRequest(responce) == E_OK)
        {
            weight_str = responce.substring(START_WEIGHT_POSITION, END_WEIGHT_POSITION);
            weight_str.trim();
            DEBUG_MSG_P(PSTR("[ICS449] #%i weight value: %s kg\n"), i+1, weight_str.c_str());
            sum_weight += atof(weight_str.c_str());
            ++numberOfSuccess_n;
        }
        else
        {
            DisplayMessage("Try again.");
        }
    }
    if(numberOfSuccess_n == 0)
    {
        DisplayMessage("Try again.");
        return INVALID_WEIGHT;
    } else
    {
        weight_str = String(sum_weight/numberOfSuccess_n);
        String weightDisplay = "Weight:" + weight_str + " kg";
        DisplayMessage(weightDisplay);
    }
    return sum_weight/numberOfSuccess_n;
}

void _sendData()
{
    String topic_str = MQTT_TOPIC_WEIGHT;
    float_t weight_f = _getWeightValue();
    g_mqtt_packet_data.weight_str = String(weight_f);

    #ifdef BALANCE_DEBUG
        DEBUG_MSG_P(PSTR("[MQTT] Start send MQTT\n"));
        mqttSend4Balance(topic_str.c_str(), g_mqtt_packet_data.weight_str.c_str());
        DEBUG_MSG_P(PSTR("[MQTT] Success.\n"));
    #else
        if(weight_f <= 0)
        {
            DEBUG_MSG_P(PSTR("Invalid weight value.\n"));
        } else
        {    
            DEBUG_MSG_P(PSTR("[MQTT] Start send MQTT\n"));
            mqttSend(topic_str.c_str(), g_mqtt_packet_data.weight_str.c_str());
            DEBUG_MSG_P(PSTR("[MQTT] Success.\n"));
        }
    #endif
}

void _eventGetWeight() {
    for (size_t id = 0; id < _buttons.size(); ++id) {
        auto event = _buttons[id].loop();
        if (event != button_event_t::None) {
            DEBUG_MSG_P(PSTR("[BTN] Button #%u event %d (%s)\n"),
            id, _buttonEventNumber(event), buttonEventString(event).c_str()
            );
            
            if (event == button_event_t::None) return;

            auto& button = _buttons[id];
            auto action = _buttonDecodeEventAction(button.actions, event);
            
            if((id == BTN_RECORD_WEIGHT)&&((action == ACTION_PRESSED)||(action == ACTION_CLICK)))
            {
                _sendData();
            }
        }
    }
}

void _sendDataWithSTMode()
{
    const String topic_str = MQTT_TOPIC_WEIGHT;
    String responce = RESPONCE_ERROR;
    float weight_f = 0.0;
    while(UART_PORT.available() > 0)
    {
        responce = UART_PORT.readString();
        DEBUG_MSG_P(PSTR("[ICS449] responce: %s\n"), responce.c_str());
        if(responce.substring(START_WEIGHT_POSITION_ST, END_WEIGHT_POSITION_ST) == RECEIVE_NET_WEIGHT_VALUE)
        {
            g_mqtt_packet_data.weight_str = responce.substring(ST_START_WEIGHT_POSITION, ST_END_WEIGHT_POSITION);
            g_mqtt_packet_data.weight_str.trim();
            weight_f = atof(g_mqtt_packet_data.weight_str.c_str());
            DEBUG_MSG_P(PSTR("[ICS449] weight value: %s kg\n"), String(weight_f).c_str());
        } else
        {
            DEBUG_MSG_P(PSTR("[MCU] Receive a unknown message: %s.\n"), responce.c_str());
        }

        if(weight_f <= 0)
        {
            DEBUG_MSG_P(PSTR("[MCU] Invalid weight value.\n"));
            DisplayMessage("Failed.");
        } else
        {    
            DEBUG_MSG_P(PSTR("[MCU] Start send data\n"));
            if(wifiConnected() == true) // can not connect to wifi
            {
                // send data to lora receiver
                Lora_SendWeight(g_mqtt_packet_data.weight_str, g_mqtt_packet_data.number_n);
            } else
            {
                // handle by MQTT
                mqttSend4Balance(topic_str.c_str(), g_mqtt_packet_data.weight_str.c_str());
                // lora should be sleep 
                lora.sleep();
            }
            DisplayMessage("Weight: " + g_mqtt_packet_data.weight_str + " kg");
            DEBUG_MSG_P(PSTR("[MCU] Get weight: success\n"));
        }
    }
}

void _reloadPondInfo()
{
    static bool run = true;
    String pondID_str = "", type_str = "";
    uint8_t lenghtOfPond = 0, lenghtOfType = 0;
    String pondInfo_str = "";
    char data_c;
    if(wsConnected()&&run)
    {
        lenghtOfPond = EEPROMr.read(PONDID_LENGHT_ADDRESS);
        lenghtOfType = EEPROMr.read(TYPE_LENGHT_ADDRESS);
        if((lenghtOfPond<=MAX_LENGHT_POND_TYPE)&&(lenghtOfType<=MAX_LENGHT_POND_TYPE)&&
        (lenghtOfPond!=0)&&(lenghtOfType!=0))
        {
            for(int i= WS_DATA_ADDRESS; i< lenghtOfPond + WS_DATA_ADDRESS + lenghtOfType; i++)
            {
                data_c = EEPROMr.read(i);
                if((i < lenghtOfPond + WS_DATA_ADDRESS)&&(pondID_str.length() < lenghtOfPond)){
                        pondID_str += data_c;
                } else if(type_str.length() < lenghtOfType)
                {
                        type_str += data_c;
                } else
                {
                    // Should not executed
                }
            }
        }
        g_mqtt_packet_data.pondID_str = pondID_str;
        g_mqtt_packet_data.type_str = type_str;

        // declare JSON object
        DynamicJsonBuffer jsonBuffer(512);
        JsonObject& root = jsonBuffer.createObject();

        // create JSON message
        root[TOPIC_WS_PONDID] = pondID_str;
        root[TOPIC_WS_TYPE] = type_str;

        // Send to output buffer
        root.printTo(pondInfo_str);
        // Clear the JSON object
        jsonBuffer.clear();

        wsSend(pondInfo_str.c_str());
        run = false;
        DEBUG_MSG_P(PSTR("[REBOOT] Reload Pond info.\n"));
        }
}

void _settingSTActive()
{
    int8_t res = E_NOT_OK;
    #ifdef BALANCE_DEBUG
    (void)_sendCMD(SEND_WEIGHT_BY_PRESSING_KEY);
    delay(50);
    #else
    while(responceSTActiveRequest(_sendCMD(INQUIRY_ACTUAL_ST_STATUS)) != E_OK)
    {
        (void)_sendCMD(SEND_WEIGHT_BY_PRESSING_KEY);
        delay(300);
        DisplayMessage("Wait for active ST funcfion.");
    }
    DisplayMessage("ST is already active.");
    DEBUG_MSG_P(PSTR("[ICS449] ST: Complete setting ST.\n"));
    #endif
}

int8_t _settingBalance()
{
    // Currently, get the setting result but not process for failure responce
    int8_t ret = E_NOT_OK;
    // reset balance
    ret *= responceSettingReset(_sendCMD(RESET_BALANCE));
    // setting COM
    // -- baudrate : 9600
    // -- 8 bits
    // -- non parity
    // -- 1 stop bit
    // -- non handshake
    #if 0
    ret *= _sendCMD(DEFAULT_COM_SETTING); // currently not use
    #endif
    /* setting RS232 */
    // start configuration, connection will be suppended
    ret *= responceRS232Setting(_sendCMD(STARTS_CONFIGURATION));
    // -- baudrate : 9600
    ret *= responceRS232Setting(_sendCMD(SETTING_RS232_BAUDRATE));
    // -- 8 bits and non parity
    ret *= responceRS232Setting(_sendCMD(SETTING_RS232_BIT));
    // -- none handshake
    ret *= responceRS232Setting(_sendCMD(SETTING_RS232_HANDSHAKE));
    // -- EOL is \r\n
    ret *= responceRS232Setting(_sendCMD(SETTING_RS232_EOL));
    // -- charset is ANSI/WIN
    #if 0
    ret *= responceRS232Setting(_sendCMD(SETTING_RS232_CHARSET));
    #endif
    // end configuration, connection will be resumed
    ret *= responceRS232Setting(_sendCMD(ENDS_CONFIGURATION));
    // zero balance immediately
    ret *= responceSettingZero(_sendCMD(ZERO_BALANCE_IMMEDIATELY));

    _settingSTActive();
    
    return ret;
}

void _checkWifiConnect()
{
    if(wifiConnected() == false)
    {
        // buzzer and LED : High
        i2cuart.digitalWrite(1, HIGH);
    } else
    {
        // buzzer and LED: Low
        i2cuart.digitalWrite(1, LOW);
    }
}

void icsSetup()
{
    #ifdef LORA_SENDER
    // setting baudrate 9600 between balance [RS232] and MCU [TTL]
    _settingCOM();
    // setting for buzzer on io extender
    _settingBuzzer();
    // lora configuration
    LoRaSetup();
    // setting balance ICS449
    #ifdef BALANCE_DEBUG
    (void)_settingBalance();
    #else
    // wait for balance configuration successfully
    while(_settingBalance() != E_OK);
    #endif
    // register loop to work
    espurnaRegisterLoop(_reloadPondInfo);

    #if 0
    espurnaRegisterLoop(_eventGetWeight);
    #endif

    espurnaRegisterLoop(_sendDataWithSTMode);
    _startInformation();
    // check wifi connection
    espurnaRegisterLoop(_checkWifiConnect);
    espurnaRegisterLoop(LoRa_Receive_Callback);
    #endif
    
    #ifdef LORA_RECEIVER
    // lora configuration
    LoRaSetup();
    // lora receiver interrupt
    espurnaRegisterLoop(LoRa_Receive_Callback);
    #endif
}