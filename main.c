#include "src/components/rom.h"
#include "src/components/ram.h"
#include "src/components/decode.h"
#include "src/components/reg.h"
#include "src/components/exec.h"
#include "src/structs.h"
#include "src/emulator.h"
#include "src/utils.h"
#include <unistd.h>


int main(int argc, char *argv[]){
	disable_cursor();  // Hide cursor block/line from the terminal
	init_term();       // No ouput from cursor
	init_end_sig();    // Fix the cursor when program finishes

	GFLAGS gflags;
	FETCH fetch;
	DECODE dcd;
	EXEC exec;

	ROM rom = rom_init();
	REG reg = reg_init();
	RAM ram = ram_init();


	update_gflags(&gflags, argc, argv);


	do {

		fetch = rom_fetch(rom);
		dcd = decode_run(fetch);
		exec = execute_run(dcd);


		// Display CPU
		emulate_cpu(rom, dcd, exec, reg, ram);

		// PC
		if(exec.upc == get_pc())
			increment_pc();
		else
			set_pc(exec.upc);


		switch(dcd.opcode) {
			case BSF_OP: case BCF_OP:
				if(exec.type == MULTI_OP){
					if(dcd.opcode == BSF_OP)
						reg.registers[exec.reg_n] |= 1 << exec.bit_n;
					else
						reg.registers[exec.reg_n] &= ~(1 << exec.bit_n);
				}
				break;
			default: break;
		}





		// Interruption
		if(gflags.stepping == 0)
			usleep(600000);  // 500ms

	} while(gflags.stepping ? getl() != 'q' : 1);

	normal_terminal();

	return 0;
}

