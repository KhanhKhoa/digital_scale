#include "LoRa_Transmission.h"
#include "ntp.h"
#include "ws.h"
#include "i2cExtender.h"
#include "mqtt.h"

SX1278 lora = new Module(LORA_CHIPSET_PIN,
                         LORA_DIO0_PIN,
                         LORA_RESET_PIN,
                         LORA_DIO1_PIN);

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

String _getType( int enumVal )
{
  return e_name_type[enumVal];
}

String _genarateLoraMessageID(){
    int run = 0, i = 0;
    String messgaeID = "";
    const char bb[] = {ID_ARRAY};
    char c;
    while(run < LENGHT_MESSAGE_ID)
    {
        i = random(RANDOM_LIMIT_MIN, RANDOM_LIMIT_MAX);
        c = bb[i];
        messgaeID += c;
        run++;
    }
    return messgaeID;
}

void _setFlag(void) 
{
    // check if the interrupt is enabled
    if(!enableInterrupt) {
        return;
    }

    // we got a packet, set the flag
    receivedFlag = true;
}

void _lora_resetup()
{
    terminalRegisterCommand(F("LORA.SEND"), [](const terminal::CommandContext&) {
        LoRa_Transmit("Test");
        terminalOK();
    });
    terminalRegisterCommand(F("LORA.SETUP"), [](const terminal::CommandContext&) {
    LoRaSetup();
    terminalOK();
    });
    // Bandwidth table: 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3
    RECONFIG_BANDWIDTH(7.8);                       
    RECONFIG_BANDWIDTH(10.4);  
    RECONFIG_BANDWIDTH(15.6);  
    RECONFIG_BANDWIDTH(20.8);  
    RECONFIG_BANDWIDTH(31.25);  
    RECONFIG_BANDWIDTH(41.7);  
    RECONFIG_BANDWIDTH(62.5);  
    RECONFIG_BANDWIDTH(125);  
    RECONFIG_BANDWIDTH(250);  
    // Frequency 
    RECONFIG_FREQUENCY(433);
    RECONFIG_FREQUENCY(433.5);
    RECONFIG_FREQUENCY(434);
    RECONFIG_FREQUENCY(434.5);
    RECONFIG_FREQUENCY(435);
    RECONFIG_FREQUENCY(435.5);
    RECONFIG_FREQUENCY(436);
    // Spreading factor: 7 - 12
    RECONFIG_SPREADING_FACTOR(7);
    RECONFIG_SPREADING_FACTOR(8);
    RECONFIG_SPREADING_FACTOR(9);
    RECONFIG_SPREADING_FACTOR(10);
    RECONFIG_SPREADING_FACTOR(11);
    RECONFIG_SPREADING_FACTOR(12);
    // coding rate
    RECONFIG_CODING_RATE(1);
    RECONFIG_CODING_RATE(2);
    RECONFIG_CODING_RATE(3);
    RECONFIG_CODING_RATE(4);
    // output power: 3 -17 dBm
    RECONFIG_OUTPUT_POWER(3);
    RECONFIG_OUTPUT_POWER(5);
    RECONFIG_OUTPUT_POWER(7);
    RECONFIG_OUTPUT_POWER(9);
    RECONFIG_OUTPUT_POWER(10);
    RECONFIG_OUTPUT_POWER(15);
    RECONFIG_OUTPUT_POWER(17);
    // over current protection limit: 45 - 240 mA
    RECONFIG_CURRENT_LIMIT(45);
    RECONFIG_CURRENT_LIMIT(60);
    RECONFIG_CURRENT_LIMIT(80);
    RECONFIG_CURRENT_LIMIT(100);
    RECONFIG_CURRENT_LIMIT(120);
    RECONFIG_CURRENT_LIMIT(140);
    RECONFIG_CURRENT_LIMIT(160);
    RECONFIG_CURRENT_LIMIT(180);
    RECONFIG_CURRENT_LIMIT(200);
    RECONFIG_CURRENT_LIMIT(220);
    RECONFIG_CURRENT_LIMIT(240);
    // preamble length
    RECONFIG_PREAMBLE(4);
    RECONFIG_PREAMBLE(6);
    RECONFIG_PREAMBLE(8);
    RECONFIG_PREAMBLE(10);
    RECONFIG_PREAMBLE(12);
    RECONFIG_PREAMBLE(14);
    // amplifier gain: 1 -6
    RECONFIG_GAIN(1);
    RECONFIG_GAIN(2);
    RECONFIG_GAIN(3);
    RECONFIG_GAIN(4);
    RECONFIG_GAIN(5);
    RECONFIG_GAIN(6);
}

void LoRaSetup()
{
    DEBUG_MSG_P(PSTR("[SX1278] Initializing ...\n"));
    int state = lora.begin();
    if (state == ERR_NONE) {
        DEBUG_MSG_P(PSTR("[SX1278] Init: Success\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Init: Failed => Error %i.\n"), state);
    }

    // set carrier frequency
    if (lora.setFrequency(LORA_FREQUENCY) == ERR_INVALID_FREQUENCY) {
        DEBUG_MSG_P(PSTR("[SX1278] Selected frequency: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Selected frequency: Success\n"));
    }

    // set bandwidth
    if (lora.setBandwidth(LORA_BANDWIDTH) == ERR_INVALID_BANDWIDTH) {
        DEBUG_MSG_P(PSTR("[SX1278] Selected bandwidth: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Selected bandwidth: Success\n"));
    }

    // set spreading factor
    if (lora.setSpreadingFactor(LORA_SPREADING) == ERR_INVALID_SPREADING_FACTOR) {
        DEBUG_MSG_P(PSTR("[SX1278] Selected spreading factor: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Selected spreading factor: Success\n"));
    }

    // set coding rate to 6
    if (lora.setCodingRate(LORA_CODING_RATE) == ERR_INVALID_CODING_RATE) {
        DEBUG_MSG_P(PSTR("[SX1278] Selected coding rate: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Selected coding rate: Success\n"));
    }

    // NOTE: value 0x34 is reserved for LoRaWAN networks and should not be used
    if (lora.setSyncWord(LORA_SYNC_WORD) != ERR_NONE) {
        DEBUG_MSG_P(PSTR("[SX1278] Set sync word: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Set sync word: Success\n"));
    }

    // set output power to 10 dBm (accepted range is -3 - 17 dBm)
    // NOTE: 20 dBm value allows high power operation, but transmission
    //       duty cycle MUST NOT exceed 1%
    if (lora.setOutputPower(LORA_OUTPUT_POWER) == ERR_INVALID_OUTPUT_POWER) {
        DEBUG_MSG_P(PSTR("[SX1278] Selected output power: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Selected output power: Success\n"));
    }

    // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
    // NOTE: set value to 0 to disable overcurrent protection
    if (lora.setCurrentLimit(LORA_CURRENT_LIMIT) == ERR_INVALID_CURRENT_LIMIT) {
        DEBUG_MSG_P(PSTR("[SX1278] Selected current limit: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Selected current limit: Success\n"));
    }

    // set LoRa preamble length to 15 symbols (accepted range is 6 - 65535)
    if (lora.setPreambleLength(LORA_PEAMBLE) == ERR_INVALID_PREAMBLE_LENGTH) {
        DEBUG_MSG_P(PSTR("[SX1278] Selected preamble length: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Selected preamble length: Success\n"));
    }

    // set amplifier gain to 1 (accepted range is 1 - 6, where 1 is maximum gain)
    // NOTE: set value to 0 to enable automatic gain control
    //       leave at 0 unless you know what you're doing
    if (lora.setGain(LORA_GAIN) == ERR_INVALID_GAIN) {
        DEBUG_MSG_P(PSTR("[SX1278] Selected gain: Invalid\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Selected gain: Success\n"));
    }

    DEBUG_MSG_P(PSTR("[SX1278] All settings successfully changed!\n"));

    #ifdef LORA_RECEIVER
    // set the function that will be called
    // when new packet is received

    lora.setDio0Action(_setFlag);
    // start listening for LoRa packets
    DEBUG_MSG_P(PSTR("[SX1278] Starting to listen ...\n"));

    state = lora.startReceive();

    if (state == ERR_NONE) {
        DEBUG_MSG_P(PSTR("[SX1278] Starting to listen: Success\n"));
    } else {
        DEBUG_MSG_P(PSTR("[SX1278] Starting to listen: Failed => Error code: %i\n"), state);
    }
    #endif
    _lora_resetup();
}

void LoRa_Transmit(String message)
{
    DEBUG_MSG_P(PSTR("[SX1278] Transmitting packet ...\n"));  
    int state = lora.transmit(message);
    DEBUG_MSG_P(PSTR("[SX1278] Message: %s\n"), message.c_str());  

    if (state == ERR_NONE) {
        // the packet was successfully transmitted
        DEBUG_MSG_P(PSTR("[SX1278] Transmit: Success\n"));  
        // print measured data rate
        DEBUG_MSG_P(PSTR("[SX1278] Datarate: %s bps\n"), String(lora.getDataRate()).c_str());

    } else if (state == ERR_PACKET_TOO_LONG) {
        // the supplied packet was longer than 256 bytes
        DEBUG_MSG_P(PSTR("[SX1278] Transmit: Too long!\n"));

    } else if (state == ERR_TX_TIMEOUT) {
        // timeout occurred while transmitting packet
        DEBUG_MSG_P(PSTR("[SX1278] Transmit: Timeout!\n"));

    } else {
        // some other error occurred
        DEBUG_MSG_P(PSTR("[SX1278] Transmit: Failed => Error code: %i\n"), state);
    }
}

void _loraParse(char* payload)
{
    // Check early for empty object / nothing
    if ((String(payload).length() == 0) || (String(payload).length() == 1)) {
        DEBUG_MSG_P(PSTR("[parsingJSON] Invalid JSON buffer\n"));
        return;
    }

    // Parse JSON input
    DynamicJsonBuffer jsonBuffer(1024);
    JsonObject& root = jsonBuffer.parseObject(payload);
    if (!root.success()) {
        DEBUG_MSG_P(PSTR("[Lora] JSON parsing error\n"));
        return; 
    }

    // Get ID
    const char* ID_c = root[LORA_TOPIC_ID];
    if(ID_c)
    {
        DEBUG_MSG_P(PSTR("[Lora] parsing ID:  %s\n"), ID_c);
        g_mqtt_packet_data.ID_str = ID_c;
    } else
    {
        DEBUG_MSG_P(PSTR("[Lora] Error when parsing ID\n"));
    }

    // Get Turn
    const char* Turn_c = root[LORA_TOPIC_TURN];
    if(Turn_c)
    {
        DEBUG_MSG_P(PSTR("[Lora] parsing Turn:  %s\n"), Turn_c);
        g_mqtt_packet_data.number_n = atoi(Turn_c);
    }  else
    {
        DEBUG_MSG_P(PSTR("[Lora] Error when parsing Turn\n"));
    }

    // Get Weight
    const char* Weight_c = root[LORA_TOPIC_WEIGHT];
    if(Weight_c)
    {
        DEBUG_MSG_P(PSTR("[Lora] parsing Weight:  %s\n"), Weight_c);
        g_mqtt_packet_data.weight_str = Weight_c;
    }  else
    {
        DEBUG_MSG_P(PSTR("[Lora] Error when parsing Weight\n"));
    }

    // Get PondID
    const char* PondID_c = root[LORA_TOPIC_POND];
    if(PondID_c)
    {
        DEBUG_MSG_P(PSTR("[Lora] parsing PondID:  %s\n"), PondID_c);
        g_mqtt_packet_data.pondID_str = PondID_c;
    }  else
    {
        DEBUG_MSG_P(PSTR("[Lora] Error when parsing PondID\n"));
    }

    // Get Type
    const char* Type_c = root[LORA_TOPIC_TYPE];
    if(Type_c)
    {
        DEBUG_MSG_P(PSTR("[Lora] parsing Type:  %s\n"), Type_c);
        if(String(Type_c) == String(e_number_type::tom_the))
        {
            g_mqtt_packet_data.type_str = _getType(e_number_type::tom_the);
        } else if(String(Type_c) == String(e_number_type::tom_su))
        {
            g_mqtt_packet_data.type_str = _getType(e_number_type::tom_su);
        } else if(String(Type_c) == String(e_number_type::other))
        {
           g_mqtt_packet_data.type_str = _getType(e_number_type::other);
        } else
        {
            // do nothing
        }
    }  else
    {
        DEBUG_MSG_P(PSTR("[Lora] Error when parsing Type\n"));
    }
}

void LoRa_Receive_Callback()
{
    // check if the flag is set
    if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;
    String g_data_str = "";
    const String topic_str = MQTT_TOPIC_WEIGHT;
    // you can read received data as an Arduino String
    int state = lora.readData(g_data_str);

    if (state == ERR_NONE) {
        // print data of the packet
        DEBUG_MSG_P(PSTR("[SX1278] Data: %s\n"), g_data_str.c_str());
        
        #ifdef LORA_RECEIVER
        // parsing packet data
        _loraParse(strdup(&g_data_str[0]));
        if((wifiConnected() == true)&&(mqttConnected() == true))
        {
            // send data to MQTT broker
            mqttSend4Balance(topic_str.c_str(), g_mqtt_packet_data.weight_str.c_str());
            LoRa_Transmit(g_mqtt_packet_data.ID_str);
        } else
        {
            // send responce to lora sender
            LoRa_Transmit(RES_ERROR_BY_WIFI_CONNECTION);
        }
        #endif

        #ifdef LORA_SENDER
        // Receive a different message ID
        if(g_data_str != g_mqtt_packet_data.ID_str)
        {
            DEBUG_MSG_P(PSTR("[Lora] Can not receive packet data\n\n"));
        } 
        else if(g_data_str == RES_ERROR_BY_WIFI_CONNECTION)
        {
            DEBUG_MSG_P(PSTR("[Lora] The receiver was not connected to wifi or MQTT broker\n\n"));
        }
        else
        {
            DEBUG_MSG_P(PSTR("[Lora] Data has been sent to MQTT broker\n\n"));
            // Increase turn number
            g_mqtt_packet_data.number_n++;
            // Turn off buzzer when ending process successfully
            turnOffBuzzer();
        }
        #endif

    } else if (state == ERR_CRC_MISMATCH) {
        // packet was received, but is malformed
        DEBUG_MSG_P(PSTR("[SX1278] CRC error\n"));

    } else {
        // some other error occurred
        DEBUG_MSG_P(PSTR("[SX1278] Receive: Failed => Error code: %i\n"), state);
    }
    #ifdef LORA_RECEIVER
    // put module back to listen mode
    lora.startReceive();
    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
    #endif
    }
}

void Lora_SendWeight(String weight, int turn)
{
    static bool isFirst = true;
    DynamicJsonBuffer jsonBuffer(1024);
    JsonObject& root = jsonBuffer.createObject();

    // genarate the message ID and stored on g_packetData.ID_str
    g_mqtt_packet_data.ID_str = _genarateLoraMessageID();

    // create JSON message
    root[LORA_TOPIC_ID] = g_mqtt_packet_data.ID_str;
    root[LORA_TOPIC_WEIGHT] = weight;
    root[LORA_TOPIC_TURN] = String(turn);
    root[LORA_TOPIC_POND] = g_mqtt_packet_data.pondID_str;
    if(g_mqtt_packet_data.type_str == _getType(e_number_type::tom_the))
    {
        root[LORA_TOPIC_TYPE] = String(e_number_type::tom_the);
    } else if(g_mqtt_packet_data.type_str == _getType(e_number_type::tom_su)) 
    {
        root[LORA_TOPIC_TYPE] = String(e_number_type::tom_su);
    } else if(g_mqtt_packet_data.type_str == _getType(e_number_type::other))
    {
        root[LORA_TOPIC_TYPE] = String(e_number_type::other);
    } else
    {
        // do nothing
    }
    // Send
    String output;
    root.printTo(output);
    jsonBuffer.clear();

    // Send this JSON data to lora receiver
    LoRa_Transmit(output);

    // just call at the first time
    if(isFirst)
    {
        lora.setDio0Action(_setFlag);
        isFirst = false;
    }
    // put module back to listen mode
    lora.startReceive();

    enableInterrupt = true;
}