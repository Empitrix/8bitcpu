#include "../utils.h"
#include "../structs.h"


/* Return a struct that contains the given instruction's information */
DECODE decode_inst(int inst){
	DECODE dcd;

	dcd.bits = dcd.addr = 0;

	// NOP
	if(inst == 0b0){
		dcd.opcode = NOP_OP;
		dcd.type = FULL_OPERAND;
		dcd.info = "NOP";

		// CLRW
	} else if(inst == 0b000001000000){
		dcd.opcode = CLRW_OP;
		dcd.type = FULL_OPERAND;
		dcd.info = "CLRW";

		// SLEEP
	} else if(inst == 0b000000000011){
		dcd.opcode = SLEEP_OP;
		dcd.type = FULL_OPERAND;
		dcd.info = "SLEEP";

		// BSF
	} else if(edfb(inst, 9, 12) == 0b0101){
		dcd.opcode = BSF_OP;
		dcd.operand = edfb(inst, 1, 8);
		dcd.type = MULTI_OPERAND;
		dcd.info = "BSF";

		dcd.bits = edfb(inst, 6, 8);
		dcd.addr = edfb(inst, 1, 5);

		// BCF
	} else if(edfb(inst, 9, 12) == 0b0100){
		dcd.opcode = BCF_OP;
		dcd.operand = edfb(inst, 1, 8);
		dcd.type = MULTI_OPERAND;
		dcd.info = "BCF";

		dcd.bits = edfb(inst, 6, 8);
		dcd.addr = edfb(inst, 1, 5);

	// MOVLW
	} else if(edfb(inst, 9, 12) == 0b1100){
		dcd.opcode = MOVLW_OP;
		dcd.operand = edfb(inst, 1, 8);
		dcd.type = MONO_OPERAND;
		dcd.info = "MOVLW";

		dcd.bits = edfb(inst, 1, 8);

	// MOVWF
	} else if(edfb(inst, 6, 12) == 0b0000001){
		dcd.opcode = MOVWF_OP;
		dcd.operand = edfb(inst, 1, 5);
		dcd.type = MONO_OPERAND;
		dcd.info = "MOVWF";

		dcd.addr = edfb(inst, 1, 5);


	// CLRF
	} else if(edfb(inst, 6, 12) == 0b0000011){
		dcd.opcode = CLRF_OP;
		dcd.operand = edfb(inst, 1, 5);
		dcd.type = MONO_OPERAND;
		dcd.info = "CLRF";
		
		dcd.addr = edfb(inst, 1, 5);

	// GOTO
	} else if(edfb(inst, 10, 12) == 0b101){
		dcd.opcode = GOTO_OP;
		dcd.operand = edfb(inst, 1, 9);
		dcd.type = MONO_OPERAND;
		dcd.info = "GOTO";
		
		dcd.addr = edfb(inst, 1, 9);

	// Undefined
	} else
		dcd.opcode = NOP_OP;

	return dcd;
}

