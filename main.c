#include "src/components/rom.h"
#include "src/components/decode.h"
#include "src/structs.h"
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[]){

	GFLAGS gflags;
	FETCH fetch;
	DECODE dcd;

	ROM rom = rom_init();

	update_gflags(&gflags, argc, argv);
	// dump_rom(rom);


	while(gflags.step_mode ? getl() != 'q' : 1){

		fetch = rom_fetch(rom);


		//printf("%d\n", fetch.data);
		dcd = decode_run(fetch);

		printf("BIN: %s - ", decimal_to_binary(fetch.data, 12));

		switch(dcd.opcode) {

			case NOP_OP:
				prtprt("[yel]NOP");
				break;

			case BCF_OP:
				prtprt("[yel]BCF");
				break;

			case BSF_OP:
				prtprt("[yel]BSF");
				break;

			case GOTO_OP:
				prtprt("[yel]GOTO: [blu]0x%x [nrm]([grn]Executed![nrm]) -> PC = %d", dcd.operand, dcd.operand);
				set_pc(dcd.operand);
				break;

			default:
				break;
		}

		if(gflags.step_mode == 0)
			usleep(500000);  // 500ms
	}

	return 0;
}

