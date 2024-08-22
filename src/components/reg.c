#include "../structs.h"


REG reg_init(){
	REG reg;

	for(int i = 0; i < REGSIZ; ++i){
		reg.registers[i] = 0;
	}

	return reg;
}

