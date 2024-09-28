#include <stdio.h>
#include "../types.h"
#include "mem.h"
#include "../display.h"


/* rom_init: setup EEPROM */
void rom_init(char *rom_path){
	memset(ROM, 0, sizeof(ROM));

	FILE *fp;
	fp = fopen(rom_path, "rb");
	if(fp == NULL)
		lprt(1, "File \"%s\" does not exist!", rom_path);

	unsigned char buffer[2];
	int value, idx;
	idx = 0;

	while (fread(buffer, sizeof(buffer), 1, fp) == 1) {
		value = (buffer[0] << 8) | buffer[1];
		ROM[idx++] = value;
	}

	fclose(fp);
}

FETCH rom_fetch(){
	FETCH f;
	f.pc = get_sfr(PCL_REGISTER);
	f.data = ROM[f.pc];
	return f;
}

void set_pc(int new_pc){
	set_sfr(PCL_REGISTER, new_pc);
}

void increment_pc(void){
	int pcl = get_sfr(PCL_REGISTER);
	set_sfr(PCL_REGISTER, pcl + 1);
}

int get_pc(void){
	// return PC;
	return get_sfr(PCL_REGISTER);
}


void reset_cpu(){
	set_pc(0);
	int i;
	for(i = 0; i < REGSIZ; ++i){
		REGISTERS[i] = 0;
	}

	for(i = 0; i < RAMSIZ; ++i){
		RAM[i] = 0;
	}
	clear_sfr_bit(STATUS_REGISTER, 7);
}
