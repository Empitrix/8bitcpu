#include "../structs.h"
#include "./decode.h"
#include "rom.h"
#include <stdio.h>


/* return EXEC that contains execute information for given instruction */
/* Soft in soft_execute means it's not memory (register, ram ...) related */
EXEC soft_execute(DECODE dcd){
	EXEC exec;

	// Default PC
	exec.upc = get_pc();

	switch(dcd.opcode) {
		case GOTO_OP:
			exec.upc = dcd.operand;
			break;

	default:
		break;
	}

	return exec;
}


/* Return execution information for the given instruction */
char* exec_info(int inst){
	char *info = malloc(MALL * sizeof(char));

	DECODE dcd = decode_inst(inst);

	char *bits_1sec = dtob(inst, 12);
	char *bits_2sec = dtob2sec(inst, "[62AEEF]", "[E5C07A]");
	char *bits_3sec = dtob3sec(inst, "[62AEEF]", "[E5C07A]", "[C678DD]");

	switch(dcd.type) {
		case FULL_OPERAND:
			sprintf(info, "[62AEEF]%s  [2979FF]%s [98C379]%s", bits_1sec, dtoh(inst, 3), dcd.info);
			break;

		case MULTI_OPERAND:
			sprintf(info, "%s[FFFFFF]  [2979FF]%s [98C379]%s [ed400e]%s[FFFFFF], [E98C31]%s", bits_3sec, dtoh(inst, 3), dcd.info, dtoh(dcd.addr, 2), dtoh(dcd.bits, 2));
			break;

		case MONO_OPERAND:
			sprintf(info, "%s[FFFFFF]  [2979FF]%s [98C379]%s [ed400e]%s", bits_2sec, dtoh(inst, 3), dcd.info, dtoh(dcd.operand, 4));
			break;

		default:
			sprintf(info, "[62AEEF]%s  [2979FF]%s [98C379]%s", bits_1sec, dtoh(inst, 3), "NOP");
			break;
	}

	strcpy(info, update_color(info, 1));
	return info;
}


/* Execute & Update Memory etc... */
void execute(DECODE dcd, REG *reg){
	switch(dcd.opcode) {

		// Set selected bit from the register to 1
		case BSF_OP:
			reg->registers[dcd.addr] |= 1 << dcd.bits;
			break;

		// Set selected bit from the register to 0
		case BCF_OP:
			reg->registers[dcd.addr] &= ~(1 << dcd.bits);
			break;

		default:
			break;
	}
}

