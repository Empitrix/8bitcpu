#include "../types.h"


void reg_init(void){
	for(int i = 0; i < REGSIZ; ++i){
		REGISTERS[i] = 0;
	}
}


char *get_reg_name(int reg_addr){
	switch (reg_addr) {
		case 0:
			return "[FFFFFF]INDF";
		case 1:
			return "[FFFFFF]TMR0";
		case 2:
			return "[FFFFFF]PCL";
		case 3:
			return "[FFFFFF]STATUS";
		case 4:
			return "[FFFFFF]FSR";
		case 5:
			return "[FFFFFF]OSCCAL";
		case 6:
			return "[FFFFFF]GPIO";
		case 7:
			return "[FFFFFF]CMCON0";
		case 8:
			return "[FFFFFF]TRISGPIO";
		case 9:
			return "[FFFFFF]OPTION";
		default:
			return "[ABB2BF]N/A";
			break;
	}
}
