#include "../structs.h"
#include "./rom.h"


void execute_run(DECODE dcd){
	int b_val, f_val;

	switch(dcd.opcode) {
		case NOP_OP:
			// prtprt("[yel]NOP:      [nrm]([grn]Executed![nrm]) -> Do Nothing");
			break;

		case BCF_OP:
			// prtprt("[yel]BCF");
			break;

		case BSF_OP:
			b_val = dcd.operand >> 5;
			f_val = dcd.operand & 0b00011111;
			// prtprt("[yel]BSF:      [nrm]([yel]Progress![nrm]) [nrm]SET [blu]B:%s[nrm] for [blu]F:%s [red]=> [blu]%d", decimal_to_binary(b_val, 3), decimal_to_binary(f_val, 5), f_val);
			break;

		case GOTO_OP:
			// prtprt("[yel]GOTO: [blu]0x%x [nrm]([grn]Executed![nrm]) -> PC = %d", dcd.operand, dcd.operand);
			set_pc(dcd.operand);
			break;

		default:
			break;
	}

}
