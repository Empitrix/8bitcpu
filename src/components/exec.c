#include "../structs.h"
#include "./rom.h"


EXEC execute_run(DECODE dcd){
	int b_val, f_val;

	EXEC exec;

	switch(dcd.opcode) {
		case NOP_OP:
			exec.opcode = "NOP";
			break;

		case BCF_OP:
			exec.opcode = "BCF";
			break;

		case BSF_OP:
			b_val = dcd.operand >> 5; // len: 3
			f_val = dcd.operand & 0b00011111; // len: 5
			exec.opcode = "BSF";
			break;

		case GOTO_OP:
			exec.opcode = "GOTO";
			set_pc(dcd.operand);
			break;

		default:
			exec.opcode = "NOP";
			break;
	}

	return exec;
}

char* execute_info(int inst){
	char *info;
	int opcode = inst >> 8;
	switch(opcode) {
		case NOP_OP:
			info = "NOP";
			break;

		case BCF_OP:
			info = "BCF";
			break;

		case BSF_OP:
			info = "BSF";
			break;

		case GOTO_OP:
			info = "GOTO";
			break;

		default:
			info = "NOP";
			break;
	}

	return info;
}
