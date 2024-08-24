#include "src/components/rom.h"
#include "src/components/ram.h"
#include "src/components/decode.h"
#include "src/components/reg.h"
#include "src/components/exec.h"
#include "src/display.h"
#include "src/structs.h"
#include "src/emulator.h"
#include "src/utils.h"
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[]){
	// hide cursor and no echo for terminal
	hidden_terminal();
	// Listen to <C-c> (END OF THE PROGRAM) & take action
	init_end_sig();


	GFLAGS gflags;
	FETCH fetch;
	DECODE dcd;
	EXEC exec;

	// Update flags
	update_gflags(&gflags, argc, argv);


	ROM rom = rom_init(gflags.program);
	REG reg = reg_init();
	RAM ram = ram_init();

	do {

		if(gflags.stepping == 0){
			if(getc_keep() == ' ')
				gflags.is_pause = ~gflags.is_pause;

			if(gflags.is_pause != 0){
				usleep(gflags.frequency);
				dprt(term_size().x - 10, 2, "[26aF9a][bl]❚❚ Paused");
				fflush(NULL);
				continue;
			}
		}

		// Capture data for each round
		fetch = rom_fetch(rom);
		dcd = decode_run(fetch);
		exec = execute_capture(dcd);

		// Display CPU
		emulate_cpu(rom, dcd, exec, reg, ram, gflags);

		// Update PC
		if(exec.upc == get_pc())
			increment_pc();
		else
			set_pc(exec.upc);

		// Update Reg & Ram
		execute(dcd, exec, &reg);

		// Interruption
		if(gflags.stepping == 0)
			usleep(gflags.frequency);

	} while(gflags.stepping ? getl() != 'q' : 1);


	// turn the echo on and make cursor visible
	normal_terminal();
	return 0;
}

