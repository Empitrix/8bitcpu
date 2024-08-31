#include <stdio.h>
#include "../structs.h"
#include "../utils.h"
#include "../display.h"

static int PC = 0;  // Main PC: Program Counter


/* rom_init: setup EEPROM */
ROM rom_init(char *rom_path){
	ROM rom;
	memset(rom.mcode, 0, sizeof(rom.mcode));

	FILE *fp;
	// fp = fopen("./rom.bin", "rb");
	fp = fopen(rom_path, "rb");
	if(fp == NULL)
		lprt(1, "File \"%s\" does not exist!", rom_path);

	unsigned char buffer[2];
	int value, idx;
	idx = 0;

	while (fread(buffer, sizeof(buffer), 1, fp) == 1) {
		value = (buffer[0] << 8) | buffer[1];
		rom.mcode[idx++] = value;
	}

	fclose(fp);
	return rom;
}

FETCH rom_fetch(ROM rom){
	FETCH f;
	// f.pc = PC++;
	f.pc = PC;
	f.data = rom.mcode[f.pc];
	return f;
}

void set_pc(int new_pc){
	PC = new_pc;
}

void increment_pc(void){
	PC++;
}

int get_pc(){
	return PC;
}

/* dump_rom: show rom's data */
void dump_rom(ROM rom){
	printf("ROM Data:\n");
	printf("| IDX | DECIMAL | HEX    | BINARY         |\n");
	printf("|-----|---------|--------|----------------|\n");
	int value, i; i = value = 0;
	for(i = 0; i < sizeof(rom.mcode) / sizeof(rom.mcode[0]); ++i){
		value = rom.mcode[i];
		printf("| %-3d | %-7d | 0x%-4x | %s |\n", i, value, value, dtob(value, 12));
	}
	printf("\n\n");
}



void reset_cpu(REG *reg, RAM *ram){
	set_pc(0);
	int i;
	for(i = 0; i < REGSIZ; ++i)
		reg->registers[i] = 0;

	for(i = 0; i < RAMSIZ; ++i)
		ram->ram[i] = 0;
}
