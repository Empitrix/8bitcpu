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

	int c = ' ';

	GFLAGS gflags;
	FETCH fetch;
	DECODE dcd;
	EXEC exec;
	ROM rom;
	REG reg;
	RAM ram;

	// Update flags
	update_gflags(&gflags, argc, argv);

	if(gflags.pload == STATE_LOAD){
		int tmp;
		tmp = load_cpu_state(&gflags, &reg, &ram);
		if(tmp < 0)
			lprt(1, "Invalid cpu state file!");
		else
			set_pc(tmp);
	}

	rom = rom_init(gflags.program);

	if(gflags.pload == PROGRAM_LOAD){
		reg = reg_init();
		ram = ram_init();
	}

	do {

		if(c == 's'){
			dprt(term_size().x - 10, 2, "   [00FF00][bl]Saved!");
			save_cpu_state(gflags, reg, ram, get_pc());
		}

		if(c != ' ')
			continue;


		if(gflags.stepping == 0){

			if(getc_keep() == ' ')
				gflags.is_pause = ~gflags.is_pause;


			if(getc_keep() == 's'){
				dprt(term_size().x - 10, 2, "   [00FF00]Saved!");
				save_cpu_state(gflags, reg, ram, get_pc());
			}

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

	} while(gflags.stepping ? (c = getl()) != 'q' : 1);


	// turn the echo on and make cursor visible
	normal_terminal();
	return 0;
}

