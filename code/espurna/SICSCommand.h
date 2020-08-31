#ifndef __SICS_COMMAND__
#define __SICS_COMMAND__

#include "espurna.h"

// Avoid multidefine of STD_RETURN_TYPE
#ifndef __STD_RETURN_TYPE__
#define __STD_RETURN_TYPE__
typedef int8_t STD_RETURN_TYPE;
#define E_OK       1        // Positive value
#define E_NOT_OK   0        // Zero
#define E_PENDING  -1       // Negative value
#endif

// ICS449 command
#define RESET_BALANCE               "@"                 // Reset balance                    
#define GET_BALANCE_ID              "I10"               // Get balance ID
#define GET_BALANCE_TYPE            "I11"               // Get balance type
#define ZERO_BALANCE_IMMEDIATELY    "ZI"                // Zero immediately
#define DEFAULT_COM_SETTING         "COM A 0 6 3 0"     // Defaut setting for COM port
#define STARTS_CONFIGURATION        "COPT \"start\""    // Start configuration for connection
#define ENDS_CONFIGURATION          "COPT \"end\""      // End configuration for connection
#define SETTING_RS232_BAUDRATE      "COPT \"set\" \"Connection.ConnectionList.1.Baudrate\" \"i4\" \"6\""    // Set baudrate 9600 for RS232
#define SETTING_RS232_BIT           "COPT \"set\" \"Connection.ConnectionList.1.Parity\" \"i4\" \"3\""      // Set none parity for RS232
#define SETTING_RS232_HANDSHAKE     "COPT \"set\" \"Connection.ConnectionList.1.Handshake\" \"i4\" \"0\""   // Set none handshake for RS232
#define SETTING_RS232_EOL           "COPT \"set\" \"Connection.ConnectionList.1.EndOfLine\" \"i4\" \"0\""   // Set EOL is /r/n for RS232
#define SETTING_RS232_CHARSET       "COPT \"set\" \"Connection.ConnectionList.1.CharSet\" \"i4\" \"0\""     // Set charset as ANSI/WIN
#define GET_SCALE_VALUE             "SI"                // Request send scale value immediately
#define SWITCH_WEIGHT_MODE          "DW"                // Switch to weight mode display
#define SETTING_BEEPER_VOLUME_MAX   "M11 100"           // Setting beeper volume to max value - 100
#define SETTING_BEEPER_VOLUME_MID   "M11 50"            // Setting beeper volume to midle value - 50
#define SETTING_BEEPER_VOLUME_MIN   "M11 0"             // Setting beeper volume to min value - 0
#define SETTING_BEEPER_TONE_1       "M12 0"             // Setting beeper tone as variant 1
#define SETTING_BEEPER_TONE_2       "M12 1"             // Setting beeper tone as variant 2
#define SETTING_BEEPER_TONE_3       "M12 2"             // Setting beeper tone as variant 3
#define SEND_WEIGHT_BY_PRESSING_KEY "ST 1"              // Request send weight value by pressing key on keyboard
#define INQUIRY_ACTUAL_ST_STATUS    "ST"                // Inquiry of actual status of the ST function

// ICS449 responce
#define WEIGHT_CMD_NOT_EXECUTABLE   "S I"               // S command not executed
#define BALANCE_OVERLOAD            "S +"               // Balance is in overload state
#define BALANCE_UNDERLOAD           "S -"               // Balance is in underload state
#define ZERO_SETTING_PERFORMED      "Z A"               // Zero was performed
#define ZERO_NOT_EXECUTABLE         "Z I"               // Zero isn't executed
#define UPPER_LIMIT_ZERO            "Z +"               // Upper limit zero
#define LOWER_LIMIT_ZERO            "Z -"               // Lower limit zero
#define READY_FOR_OPERATION         "I4 A"              // Ready for operation
#define TEXT_APPEARS_SUCCESS        "D A"               // Text appears successfully
#define CMD_DIS_NOT_EXECUTABLE      "D I"               // D command isn't executed
#define NO_DISPLAY_BY_THE_REASON    "D L"               // Can't display the text
#define SWITCH_WEIGHT_MODE_SUCCESS  "DW A"              // Switch to weight mode display successfully
#define SWITCH_WEIGHT_MODE_FAILED   "DW I"              // Switch to weight mode display failure 
#define COM_SETTING_SUCCESS         "COM A 0 6 3 0"     // This setting is the same with the factory setting
#define COM_SETTING_FAILED          "COM L"             // Setting COM is failed
#define START_CONFIG_SUCCESS        "COPT A \"\""       // Start configuration successfully
#define RS232_CONFIG_SUCCESS        "COPT A \"\""       // Configuration for connection successfully
#define END_CONFIG_SUCCESS          "COPT A \"\""       // End configuration successfully
#define BEEPER_VOLUME_EXECUTED      "M11 A"             // M11 command was executed
#define BEEPER_VOLUME_WRONG_PARAM   "M11 L"             // M11 command with wrong parameter
#define BEEPER_VOLUME_NOT_EXECUTED  "M11 I"             // M11 command isn't executed
#define BEEPER_TONE_EXECUTED        "M12 A"             // M12 command was executed
#define BEEPER_TONE_WRONG_PARAM     "M12 L"             // M12 command with wrong parameter
#define BEEPER_TONE_NOT_EXECUTED    "M12 I"             // M12 command isn't executed
#define ST_FUNCTION_IS_INACTIVE     "ST A 0"            // ST function is in inactive state
#define ST_FUNCTION_IS_ACTIVE       "ST A 1"            // ST function is in active state
#define ST_FUNCTION_SUCCESSFULLY    "ST A"              // ST command was executed
#define ST_FUNCTION_NOT_EXECUTED    "ST I"              // ST function isn't exxcuted
#define ST_FUNCTION_WRONG_PARAMETER "ST L"              // ST function with wrong parameter
#define RECEIVE_NET_WEIGHT_VALUE    "S S"               // Receive net weight value
#define RESPONCE_ERROR              "ERROR"

STD_RETURN_TYPE responceWeightRequest(String responce);

STD_RETURN_TYPE responceSettingZero(String responce);

STD_RETURN_TYPE responceSettingReset(String responce);

STD_RETURN_TYPE responceTextDisplay(String responce);

STD_RETURN_TYPE responceRS232Setting(String responce);

STD_RETURN_TYPE responceSTActiveRequest(String responce);

#endif