#include "../structs.h"


RAM ram_init(void){
	RAM ram;
	for(int i = 0; i < RAMSIZ; ++i)
		ram.ram[i] = 0x00;
	return ram;
}

