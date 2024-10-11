#ifndef __RULES_HEADER__
#define __RULES_HEADER__

#define ROMSIZ 256   // 256 bytes of ROM (Addresses 0x00..0xFF)
#define RAMSIZ 16    // 16 bytes of RAM (Addresses 0x10..0x1F)
#define REGSIZ 10    // 10 Special Registers (Addresses 0x00..0x09)

#define MALL 1000    // Max Assembly Line Length

#define MAXSIZ 1024  // String size

#define MAX_STACK 2  // Max Stack (CALL & RETLW)

#define KB_P1  "[1388d4]"
#define KB_P2  "[e63c07]"
#define KB_P3  "[14c45b]"

#define K_HEX  "[2979FF]"
#define K_INFO "[98C379]"

#define K_OP1 "[ed400e]"
#define K_OP2 "[E98C31]"

#endif
