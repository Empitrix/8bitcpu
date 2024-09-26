#ifndef __RULES_HEADER__
#define __RULES_HEADER__

#define ROMSIZ 256   // 256 bytes of ROM (Addresses 0x00..0xFF)
#define RAMSIZ 16    // 16 bytes of RAM (Addresses 0x10..0x1F)
#define REGSIZ 10    // 10 Special Registers (Addresses 0x00..0x09)

#define MALL 1000    // Max Assembly Line Length

#define MAX_STACK 2  // Max Stack (CALL & RETLW)

/* Registers */
// #define INDF_REGISTER      0
// #define TMR0_REGISTER      1
// #define PCL_REGISTER       2
// #define STATUS_REGISTER    3
// #define FSR_REGISTER       4
// #define OSCCAL_REGISTER    5
// #define GPIO_REGISTER      6
// #define CMCON0_REGISTER    7
// #define TRISGPIO_REGISTER  8
// #define OPTION_REGISTER    9

#endif
