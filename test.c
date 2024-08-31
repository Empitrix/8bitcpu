#include "src/components/decode.h"
#include <stdio.h>

int main(void){
	inst_to_opcode(0b010100000000);  // BSF
	inst_to_opcode(0b010000000000);  // BCF
	inst_to_opcode(0b101000000000);  // GOTO
	inst_to_opcode(0b000000000000);  // NOP
	inst_to_opcode(0b110000000000);  // MOVLW
	inst_to_opcode(0b000000100000);  // MOVWF
	inst_to_opcode(0b000001100000);  // CLRF
	inst_to_opcode(0b000001000000);  // CLRW
	inst_to_opcode(0b000000000011);  // SLEEP
	printf("Finished...\n");
	return 0;
}
