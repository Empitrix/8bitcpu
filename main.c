#include "src/components.h"

int main(void){
	ROM rom = rom_init();
	dump_rom(rom);
	return 0;
}

