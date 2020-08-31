#include "SICSCommand.h"
#include "digitalScale.h"

STD_RETURN_TYPE responceWeightRequest(String responce)
{
    int8_t ret = E_NOT_OK;
    if(responce == WEIGHT_CMD_NOT_EXECUTABLE)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Error: Command not executable \
        (balance is currently executing another command, e.g. taring).\n"));
    } else if(responce == BALANCE_OVERLOAD)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Error: Balance in overload range.\n"));
    } else if(responce == BALANCE_UNDERLOAD)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Error: Balance in underload range.\n"));
    } else 
    {
        // todo: should check with format [SI weight_value Unit]
        ret = E_OK;
    }
    return ret;
}

STD_RETURN_TYPE responceSettingZero(String responce)
{
    int8_t ret = E_NOT_OK;
    if(responce == ZERO_SETTING_PERFORMED)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Zero setting performed.\n"));
        ret = E_OK;
    } else if(responce == ZERO_NOT_EXECUTABLE)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Zero: Can not executable the zero command right now.\n"));
    } else if(responce == UPPER_LIMIT_ZERO)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Zero: Upper limit of zero.\n"));
    } else if(responce == LOWER_LIMIT_ZERO)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Zero: Lower limit of zero.\n"));
    } else
    {
        ret = E_PENDING;
    }
    return ret;
}

STD_RETURN_TYPE responceSettingReset(String responce)
{
    int8_t ret = E_NOT_OK;
    if(responce.substring(0,4) == READY_FOR_OPERATION)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Ready for operation.\n"));
        ret = E_OK;
    }
    else 
    {
        DEBUG_MSG_P(PSTR("[ICS449] Wait for execution.\n"));
        ret = E_PENDING;
    }
    return ret;
}

STD_RETURN_TYPE responceTextDisplay(String responce)
{
    int8_t ret = E_NOT_OK;
    if(responce == TEXT_APPEARS_SUCCESS)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Text appears unabridged left-aligned in the balance \
        display marked by a symbol, e.g. *.\n"));
        ret = E_OK;
    } else if(responce == CMD_DIS_NOT_EXECUTABLE)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Text appears: Command not executable.\n"));
    } else if(responce == NO_DISPLAY_BY_THE_REASON)
    {
        DEBUG_MSG_P(PSTR("[ICS449] Text appears: Command understood, parameter wrong or \
        balance with no display.\n"));  
    } else
    {
        DEBUG_MSG_P(PSTR("[ICS449] Text appears: recieved a unknown responce.\n")); 
        ret = E_PENDING;
    }
    return ret;
}

STD_RETURN_TYPE responceCOMSetting(String responce)
{
    int8_t ret = E_NOT_OK;
    if(responce == COM_SETTING_SUCCESS)
    {
        DEBUG_MSG_P(PSTR("[ICS449] COM setting: Success.\n")); 
        ret = E_OK;
    } else if (responce == COM_SETTING_FAILED)
    {
        DEBUG_MSG_P(PSTR("[ICS449] COM setting: Failed.\n")); 
    } else
    {
        DEBUG_MSG_P(PSTR("[ICS449] COM setting: Unkown responce.\n"));
        ret = E_PENDING;
    }
    return ret;
}

STD_RETURN_TYPE responceRS232Setting(String responce)
{
    int8_t ret = E_OK;
    if(responce == START_CONFIG_SUCCESS)
    {
        DEBUG_MSG_P(PSTR("[ICS449] RS232 setting: Start confiuraton is success.\n"));
    } else if(responce == END_CONFIG_SUCCESS)
    {
        DEBUG_MSG_P(PSTR("[ICS449] RS232 setting: End confiuraton is success.\n"));
    } else if(responce == RS232_CONFIG_SUCCESS)
    {
        DEBUG_MSG_P(PSTR("[ICS449] RS232 setting: Success.\n"));
    } else
    {
        DEBUG_MSG_P(PSTR("[ICS449] RS232 setting: Failed.\n"));
        ret = E_NOT_OK;
    }
    return ret;
}

STD_RETURN_TYPE responceSTActiveRequest(String responce)
{
    int8_t ret = E_NOT_OK;
    if(responce == ST_FUNCTION_IS_ACTIVE)
    {
        DEBUG_MSG_P(PSTR("[ICS449] ST: Function active, weight value is sent \
        each time when F (transfer key) is pressed.\n"));
        ret = E_OK;
    } else if(responce == ST_FUNCTION_IS_INACTIVE)
    {
        DEBUG_MSG_P(PSTR("[ICS449] ST: Function inactive, no weight value is \
        sent when F (transfer key) is pressed.\n"));
        ret = E_NOT_OK;
    } else if(responce == ST_FUNCTION_NOT_EXECUTED)
    {
        DEBUG_MSG_P(PSTR("[ICS449] ST: Command understood, but not executable at present, \
        e.g. balance is currently executing another function.\n"));
        ret = E_NOT_OK;
    } else if(responce == ST_FUNCTION_WRONG_PARAMETER)
    {
        DEBUG_MSG_P(PSTR("[ICS449] ST: Command understood, parameter wrong.\n"));
        ret = E_NOT_OK;
    } else
    {
        DEBUG_MSG_P(PSTR("[ICS449] ST: A unkown responce.\n"));
        ret = E_PENDING;
    }
    return ret;
}



