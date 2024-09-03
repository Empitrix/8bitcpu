#include "../structs.h"

static int WReg = 0;


// MEM_OUT get_mem(REG *reg, RAM *ram, int addr){
MEM_OUT get_mem(REG *reg, RAM *ram, int addr){
	MEM_OUT mem;
	mem.valid = 1;

	if(addr <= (REGSIZ - 1)){
		mem.type = TO_REG;
		mem.addr = addr;
		mem.value = reg->registers[mem.addr];
		
	} else if (addr >= REGSIZ && addr <= (RAMSIZ - 1)){
		mem.type = TO_RAM;
		mem.addr = addr - REGSIZ;
		mem.value = ram->ram[mem.addr];
	} else {
		mem.valid = 0;
	}

	return mem;
}


int set_mem(REG *reg, RAM *ram, MEM_OUT mem, int val){
	if(mem.valid == 0){
		return 1;
	}
	if(mem.type == TO_REG){
		reg->registers[mem.addr] = val;
		return 0;
	} else if (mem.type == TO_RAM){
		ram->ram[mem.addr] = val;
		return 0;
	}
	return 1;
}


void set_w_reg(int val){
	WReg = val;
}

int get_w_reg(){
	return WReg;
}

