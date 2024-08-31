#include "../structs.h"
#include "../utils.h"

DECODE inst_to_opcode(int inst);

DECODE decode_run(FETCH fetch){
	int inst = fetch.data;
	// DECODE dcd = {0, 0};
	// dcd.opcode = instruction >> 8;     // OpCode
	// dcd.operand = instruction & 0xFF;  // Operand

	return inst_to_opcode(inst);
}



// OPCODES inst_to_opcode(int inst){
// 	OPCODES opcode;
// 	char buff[13];
// 	strcpy(buff, str_slice(dtob(inst, 12), 2, 14));
// 	printf("%s\n", buff);
// 
// 	if(strcmp(str_slice(buff, 0, 12), "000000000000") == 0)
// 		opcode = NOP_OP;
// 	else if(strcmp(str_slice(buff, 0, 12), "000001000000") == 0)
// 		opcode = CLRW_OP;
// 	else if(strcmp(str_slice(buff, 0, 12), "000000000011") == 0)
// 		opcode = SLEEP_OP;
// 	else if(strcmp(str_slice(buff, 0, 4), "0101") == 0)
// 		opcode = BSF_OP;
// 	else if(strcmp(str_slice(buff, 0, 4), "1100") == 0)
// 		opcode = MOVLW_OP;
// 	else if(strcmp(str_slice(buff, 0, 7), "0000001") == 0)
// 		opcode = MOVWF_OP;
// 	else if(strcmp(str_slice(buff, 0, 7), "0000011") == 0)
// 		opcode = CLRF_OP;
// 	else if(strcmp(str_slice(buff, 0, 4), "0100") == 0)
// 		opcode = BCF_OP;
// 	else if(strcmp(str_slice(buff, 0, 3), "101") == 0)
// 		opcode = GOTO_OP;
// 	else
// 		opcode = NOP_OP;
// 
// 	return opcode;
// }


DECODE inst_to_opcode(int inst){
	OPCODES opcode;
	int operand = 0;

	// NOP
	if(inst == 0b0){
		opcode = NOP_OP;

		// CLRW
	} else if(inst == 0b000001000000){
		opcode = CLRW_OP;

		// SLEEP
	} else if(inst == 0b000000000011){
		opcode = SLEEP_OP;

		// BSF
	} else if(edfb(inst, 9, 12) == 0b0101){
		opcode = BSF_OP;
		operand = edfb(inst, 1, 8);

		// BCF
	} else if(edfb(inst, 9, 12) == 0b0100){
		opcode = BCF_OP;
		operand = edfb(inst, 1, 8);

	// MOVLW
	} else if(edfb(inst, 9, 12) == 0b1100){
		opcode = MOVLW_OP;
		operand = edfb(inst, 1, 8);

	// MOVWF
	} else if(edfb(inst, 6, 12) == 0b0000001){
		opcode = MOVWF_OP;
		operand = edfb(inst, 1, 5);

	// CLRF
	} else if(edfb(inst, 6, 12) == 0b0000011){
		opcode = CLRF_OP;
		operand = edfb(inst, 1, 5);

	// GOTO
	} else if(edfb(inst, 10, 12) == 0b101){
		opcode = GOTO_OP;
		operand = edfb(inst, 1, 9);

	// Undefined
	} else
		opcode = NOP_OP;

	return (DECODE){opcode, operand};
}


