#include "../structs.h"
#include "./rom.h"


EXEC execute_run(DECODE dcd){

	EXEC exec;

	exec.upc = get_pc();          // default PC
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
			// set_pc(dcd.operand);
			break;

	default:
			exec.opcode = "NOP";
			exec.type = MONO_OP;
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
