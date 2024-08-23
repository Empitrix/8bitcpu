#include "../structs.h"


REG reg_init(){
	REG reg;

	for(int i = 0; i < REGSIZ; ++i){
		reg.registers[i] = 0;
	}

	return reg;
}


char *get_reg_name(int reg_addr){
	char *name = "";
	switch (reg_addr) {
		case 6:
			name = "[FFFFFF]GPIO";
			break;
		default:
			name = "[ABB2BF]RESEVERD";
			break;
	}
	return name;
}
