#include "i2cExtender.h"
/*
    Table 32. SC16IS740/750/760 address map
    A1  A0  IS750 I2C addresses (hex)[1]
    VDD VDD 0x90 (1001 000X)
    VDD VSS 0x92 (1001 001X)
    VDD SCL 0x94 (1001 010X)
    VDD SDA 0x96 (1001 011X)
    VSS VDD 0x98 (1001 100X)
    VSS VSS 0x9A (1001 101X)
    VSS SCL 0x9C (1001 110X)
    VSS SDA 0x9E (1001 111X)
    SCL VDD 0xA0 (1010 000X)
    SCL VSS 0xA2 (1010 001X)
    SCL SCL 0xA4 (1010 010X)
    SCL SDA 0xA6 (1010 011X)
    SDA VDD 0xA8 (1010 100X)
    SDA VSS 0xAA (1010 101X)
    SDA SCL 0xAC (1010 110X)
    SDA SDA 0xAE (1010 111X)
    
    [1] X = logic 0 for write cycle; X = logic 1 for read cycle.
*/
// A1 -> VDD
// A0 -> VDD
// I2C address: 0X90 for write and 0X91 for read
// <=> SC16IS750_ADDRESS_AA
SC16IS750 i2cuart = SC16IS750(SC16IS750_PROTOCOL_I2C,SC16IS750_ADDRESS_AA);
