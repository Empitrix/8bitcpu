#include "../types.h"


void ram_init(void){
	for(int i = 0; i < RAMSIZ; ++i)
		RAM[i] = 0x00;
}

