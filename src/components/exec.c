#include "../structs.h"
#include "./rom.h"
#include "./decode.h"
#include <stdio.h>


/* return EXEC that contains execute information for given instruction */
EXEC execute_capture(DECODE dcd){
	EXEC exec;

	exec.upc = get_pc();          // default PC (current)
	exec.reg_n = exec.bit_n = 0;  // Register & Bit

	switch(dcd.opcode) {
		case NOP_OP:
			exec.opcode = "NOP";
			exec.type = MONO_OP;
			break;

		case BCF_OP:
			exec.bit_n = dcd.operand >> 5;          // len: 3
			exec.reg_n = dcd.operand & 0b00011111;  // len: 5
			exec.opcode = "BCF";
			exec.type = MULTI_OP;
			break;

		case BSF_OP:
			exec.bit_n = dcd.operand >> 5;          // len: 3
			exec.reg_n = dcd.operand & 0b00011111;  // len: 5
			exec.opcode = "BSF";
			exec.type = MULTI_OP;
			break;

		case GOTO_OP:
			exec.opcode = "GOTO";
			exec.type = MONO_OP;
			exec.upc = dcd.operand;
			break;

	default:
			exec.opcode = "NOP";
			exec.type = MONO_OP;
			break;
	}

	return exec;
}


/* Return execution information for given ROM instruction */
char* exec_info(int inst){
	char *info = malloc(MALL * sizeof(char));
	int b, f = 0;

	// int opcode = inst >> 8;
	// int operand = inst & 0xFF;  // Operand
	DECODE dcd = inst_to_opcode(inst);

	char *bits_1sec = dtob(inst, 12);
	char *bits_2sec = dtob2sec(inst, "[62AEEF]", "[E5C07A]");
	char *bits_3sec = dtob3sec(inst, "[62AEEF]", "[E5C07A]", "[C678DD]");

	switch(dcd.opcode) {
		case NOP_OP:
			sprintf(info, "[62AEEF]%s  [2979FF]%s [98C379]%s", bits_1sec, dtoh(inst, 3), "NOP");
			break;

		case BCF_OP:
			b = dcd.operand >> 5;
			f = dcd.operand & 0b00011111;
			sprintf(info, "%s[FFFFFF]  [2979FF]%s [98C379]%s [ed400e]%s[FFFFFF], [E98C31]%s", bits_3sec, dtoh(inst, 3), "BCF", dtoh(f, 2), dtoh(b, 2));
			break;

		case BSF_OP:
			b = dcd.operand >> 5;
			f = dcd.operand & 0b00011111;
			sprintf(info, "%s[FFFFFF]  [2979FF]%s [98C379]%s [ed400e]%s[FFFFFF], [E98C31]%s", bits_3sec, dtoh(inst, 3), "BSF", dtoh(f, 2), dtoh(b, 2));
			break;

		case GOTO_OP:
			sprintf(info, "%s[FFFFFF]  [2979FF]%s [98C379]%s [ed400e]%s", bits_2sec, dtoh(inst, 3), "GOTO", dtoh(dcd.operand, 4));
			break;

		default:
			sprintf(info, "[62AEEF]%s  [2979FF]%s [98C379]%s", bits_1sec, dtoh(inst, 3), "NOP");
			break;
	}

	strcpy(info, update_color(info, 1));
	return info;
}


/* Execute & Update BUS */
void execute(DECODE dcd, EXEC exec, REG *reg){
	switch(dcd.opcode) {

		// Set selected bit from the register to 1
		case BSF_OP:
			reg->registers[exec.reg_n] |= 1 << exec.bit_n;
			break;

		// Set selected bit from the register to 0
		case BCF_OP:
			reg->registers[exec.reg_n] &= ~(1 << exec.bit_n);
			break;

		default:
			break;
	}
}

