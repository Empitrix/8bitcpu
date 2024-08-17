#include "../structs.h"
// #include "../utils.h"


DECODE decode_run(FETCH fetch){
	int instruction = fetch.data;
	DECODE dcd = {0, 0};

	dcd.opcode = instruction >> 8;     // OpCode
	dcd.operand = instruction & 0xFF;  // Operand

	// switch (dcd.opcode) {
	// 	case NOP_OP:  // 0101 (NOP)
	// 		prtprt("[grn]NOP: [blu]%3s", decimal_to_binary(dcd.operand, 8));
	// 		break;
	// 	case BCF_OP:  // 0101 (BCF)
	// 		prtprt("[grn]BCF: [blu]%3s", decimal_to_binary(dcd.operand, 8));
	// 		break;
	// 	case BSF_OP:  // 0101 (BSF)
	// 		prtprt("[grn]BSF: [blu]%3s", decimal_to_binary(dcd.operand, 8));
	// 		break;
	// 	case GOTO_OP:  // 101K (GOTO)
	// 		prtprt("[grn]GOTO: [blu]%s", decimal_to_binary(dcd.operand, 8));
	// 		break;
	// 	default:
	// 		break;
	// }

	return dcd;
}
