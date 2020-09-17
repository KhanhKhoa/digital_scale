#ifndef __LORA_TRANSMISSION_H__
#define __LORA_TRANSMISSION_H__

#include "espurna.h"
#include <RadioLib.h>

#define LORA_FREQUENCY      433
#define LORA_BANDWIDTH      250
#define LORA_SPREADING      10
#define LORA_CODING_RATE    6
#define LORA_OUTPUT_POWER   10
#define LORA_CURRENT_LIMIT  80
#define LORA_PEAMBLE        15
#define LORA_GAIN           1
#define LORA_SYNC_WORD      0x89

// SX1278 has the following connections:
// NSS pin:   4
// DIO0 pin:  5
// RESET pin: 9
// DIO1 pin:  15
#define LORA_CHIPSET_PIN    4
#define LORA_DIO0_PIN       5
#define LORA_DIO1_PIN       15
#define LORA_RESET_PIN      9  // currently not use

#define LENGHT_MESSAGE_ID   3
#define ID_ARRAY            "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890"
#define RANDOM_LIMIT_MAX    62
#define RANDOM_LIMIT_MIN    0


#define LORA_TOPIC_ID       "I"
#define LORA_TOPIC_WEIGHT   "W"
#define LORA_TOPIC_TURN     "T"
#define LORA_TOPIC_POND     "P"
#define LORA_TOPIC_TYPE     "Y"

#define TYPE_0  "Tôm thẻ"
#define TYPE_1  "Tôm sú"
#define TYPE_2  "Khác"

#define RES_ERROR_BY_WIFI_CONNECTION    "ERROR"

extern SX1278 lora;

enum e_number_type { 
    tom_the = 0, 
    tom_su, 
    other 
};

const String e_name_type[] = { TYPE_0, TYPE_1, TYPE_2 };

// set bandwidth
#define RECONFIG_BANDWIDTH(bandwidth);                                              \
terminalRegisterCommand(("LORA.BANDWIDTH." + String(bandwidth)),                    \
[](const terminal::CommandContext&) {                                               \
    if (lora.setBandwidth(bandwidth) == ERR_INVALID_BANDWIDTH) {                    \
        DEBUG_MSG_P(PSTR("[SX1278] Selected bandwidth: Invalid\n"));                \
    } else {                                                                        \
        DEBUG_MSG_P(PSTR("[SX1278] Selected bandwidth: Success\n"));                \
        DEBUG_MSG_P(PSTR("[SX1278] Current bandwidth: %s kHz\n"),                   \
        String(bandwidth).c_str());                                                 \
    }                                                                               \
    terminalOK();                                                                   \
});                                                                                 \

// set frequency
#define RECONFIG_FREQUENCY(frequency);                                              \
terminalRegisterCommand(("LORA.FREQUENCY." + String(frequency)),                    \
[](const terminal::CommandContext&) {                                               \
    if (lora.setFrequency(frequency) == ERR_INVALID_FREQUENCY) {                    \
        DEBUG_MSG_P(PSTR("[SX1278] Selected frequency: Invalid\n"));                \
    } else {                                                                        \
        DEBUG_MSG_P(PSTR("[SX1278] Selected frequency: Success\n"));                \
        DEBUG_MSG_P(PSTR("[SX1278] Current frequency: %s MHz\n"),                   \
        String(frequency).c_str());                                                 \
    }                                                                               \
    terminalOK();                                                                   \
});

// set spreading factor
#define RECONFIG_SPREADING_FACTOR(spreading_factor);                                \
terminalRegisterCommand(("LORA.SPREADING_FACTOR." + String(spreading_factor)),      \
[](const terminal::CommandContext&) {                                               \
    if (lora.setSpreadingFactor(spreading_factor) == ERR_INVALID_SPREADING_FACTOR){ \
        DEBUG_MSG_P(PSTR("[SX1278] Selected spreading factor: Invalid\n"));         \
    } else {                                                                        \
        DEBUG_MSG_P(PSTR("[SX1278] Selected spreading factor: Success\n"));         \
        DEBUG_MSG_P(PSTR("[SX1278] Current spreading factor: %s\n"),                \
        String(spreading_factor).c_str());                                          \
    }                                                                               \
    terminalOK();                                                                   \
});

// set coding rate
#define RECONFIG_CODING_RATE(coding_rate);                                         \
terminalRegisterCommand(("LORA.CODING_RATE." + String(coding_rate)),               \
[](const terminal::CommandContext&) {                                              \
    if (lora.setCodingRate(coding_rate) == ERR_INVALID_CODING_RATE) {              \
        DEBUG_MSG_P(PSTR("[SX1278] Selected coding rate: Invalid\n"));             \
    } else {                                                                       \
        DEBUG_MSG_P(PSTR("[SX1278] Selected coding rate: Success\n"));             \
        DEBUG_MSG_P(PSTR("[SX1278] Current coding rate: %s\n"),                    \
        String(coding_rate).c_str());                                              \
    }                                                                              \
    terminalOK();                                                                  \
});

// Sync word
#define RECONFIG_SYNC_WORD(sync_word);                                             \
terminalRegisterCommand(("LORA.SYNC_WORD." + String(sync_word)),                   \
[](const terminal::CommandContext&) {                                              \
    if (lora.setSyncWord(sync_word) != ERR_NONE) {                                 \
        DEBUG_MSG_P(PSTR("[SX1278] Set sync word: Invalid\n"));                    \
    } else {                                                                       \
        DEBUG_MSG_P(PSTR("[SX1278] Set sync word: Success\n"));                    \
        DEBUG_MSG_P(PSTR("[SX1278] Current sync word: %s\n"),                      \
        String(sync_word).c_str());                                                \
    }                                                                              \
    terminalOK();                                                                  \
});

// set output power to 10 dBm (accepted range is -3 - 17 dBm)
// NOTE: 20 dBm value allows high power operation, but transmission
//       duty cycle MUST NOT exceed 1%
#define RECONFIG_OUTPUT_POWER(output_power);                                        \
terminalRegisterCommand(("LORA.OUTPUT_POWER." + String(output_power)),              \
[](const terminal::CommandContext&) {                                               \
        if (lora.setOutputPower(output_power) == ERR_INVALID_OUTPUT_POWER) {        \
            DEBUG_MSG_P(PSTR("[SX1278] Selected output power: Invalid\n"));         \
        } else {                                                                    \
            DEBUG_MSG_P(PSTR("[SX1278] Selected output power: Success\n"));         \
            DEBUG_MSG_P(PSTR("[SX1278] Current output power: %s\n"),                \
            String(output_power).c_str());                                          \
        }                                                                           \
    terminalOK();                                                                   \
});

// set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
// NOTE: set value to 0 to disable overcurrent protection
#define RECONFIG_CURRENT_LIMIT(current_limit);                                      \
terminalRegisterCommand(("LORA.CURRENT_LIMIT." + String(current_limit)),            \
[](const terminal::CommandContext&) {                                               \
        if (lora.setCurrentLimit(current_limit) == ERR_INVALID_CURRENT_LIMIT) {     \
            DEBUG_MSG_P(PSTR("[SX1278] Selected current limit: Invalid\n"));        \
        } else {                                                                    \
            DEBUG_MSG_P(PSTR("[SX1278] Selected current limit: Success\n"));        \
            DEBUG_MSG_P(PSTR("[SX1278] Current over current limit: %s\n"),          \
            String(current_limit).c_str());                                         \
        }                                                                           \
    terminalOK();                                                                   \
});

// set LoRa preamble length to 15 symbols (accepted range is 6 - 65535)
#define RECONFIG_PREAMBLE(preamble);                                                \
terminalRegisterCommand(("LORA.PREAMBLE." + String(preamble)),                      \
[](const terminal::CommandContext&) {                                               \
    if (lora.setPreambleLength(preamble) == ERR_INVALID_PREAMBLE_LENGTH) {          \
        DEBUG_MSG_P(PSTR("[SX1278] Selected preamble length: Invalid\n"));          \
    } else {                                                                        \
        DEBUG_MSG_P(PSTR("[SX1278] Selected preamble length: Success\n"));          \
        DEBUG_MSG_P(PSTR("[SX1278] Current preamble: %s\n"),                        \
        String(preamble).c_str());                                                  \
    }                                                                               \
    terminalOK();                                                                   \
});

// set amplifier gain to 1 (accepted range is 1 - 6, where 1 is maximum gain)
// NOTE: set value to 0 to enable automatic gain control
//       leave at 0 unless you know what you're doing
#define RECONFIG_GAIN(gain);                                                        \
terminalRegisterCommand(("LORA.GAIN." + String(gain)),                              \
[](const terminal::CommandContext&) {                                               \
        if (lora.setGain(gain) == ERR_INVALID_GAIN) {                               \
            DEBUG_MSG_P(PSTR("[SX1278] Selected gain: Invalid\n"));                 \
        } else {                                                                    \
            DEBUG_MSG_P(PSTR("[SX1278] Selected gain: Success\n"));                 \
            DEBUG_MSG_P(PSTR("[SX1278] Current gain: %s\n"), String(gain).c_str()); \
        }                                                                           \
    terminalOK();                                                                   \
});

void LoRaSetup();

void LoRa_Transmit(String message);

void LoRa_Receive_Callback();

void Lora_SendWeight(String weight, int turn);

#endif
