#include "../structs.h"

static int WReg = 0;
static int OPTION_REG = 0;
static int CALL_PC = 0;
static int IN_CALL = 0;
static int CARRY_VAL = 0;


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



void set_in_call(int ord){ IN_CALL = ord; }
int get_in_call(void){ return IN_CALL; }

void save_call_pc(int val){ CALL_PC = val; }
int get_call_pc(){ return CALL_PC; }



void set_option_reg(int val){ OPTION_REG = val; }
// int get_option_reg(){ return OPTION_REG; }



/* Rotate And Carry */

void set_carry(int val){ CARRY_VAL = val; }
int get_carry(){ return CARRY_VAL; }



int rotate_left_carry(uint8_t value){
	set_carry((value & 0x80) >> 7);
	return (value << 1) | (get_carry() & 0x01);
}

int rotate_right_carry(uint8_t value) {
	set_carry(value & 1);
	return (value >> 1) | (get_carry() << 7);
}


