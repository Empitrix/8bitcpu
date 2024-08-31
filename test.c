#include "src/components/decode.h"
#include <stdio.h>

int main(void){
	DECODE dcd;
	dcd = decode_inst(0b010100000000);  // BSF
	printf("%s\n", dcd.info);

	dcd = decode_inst(0b010000000000);  // BCF
	printf("%s\n", dcd.info);

	dcd = decode_inst(0b101000000000);  // GOTO
	printf("%s\n", dcd.info);

	dcd = decode_inst(0b000000000000);  // NOP
	printf("%s\n", dcd.info);

	dcd = decode_inst(0b110000000000);  // MOVLW
	printf("%s\n", dcd.info);

	dcd = decode_inst(0b000000100000);  // MOVWF
	printf("%s\n", dcd.info);

	dcd = decode_inst(0b000001100000);  // CLRF
	printf("%s\n", dcd.info);

	dcd = decode_inst(0b000001000000);  // CLRW
	printf("%s\n", dcd.info);

	dcd = decode_inst(0b000000000011);  // SLEEP
	printf("%s\n", dcd.info);

	printf("Finished...\n");
	return 0;
}
