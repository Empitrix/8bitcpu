#include "src/components/rom.h"
#include "src/components/decode.h"
#include "src/components/exec.h"
#include "src/structs.h"
#include "src/emulator.h"
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

	update_gflags(&gflags, argc, argv);


	do {
		fetch = rom_fetch(rom);
		dcd = decode_run(fetch);
		exec = execute_run(dcd);

		emulate_cpu(rom, exec);


		if(gflags.stepping == 0)
			usleep(600000);  // 500ms


		if(exec.upc == get_pc())
			increment_pc();
		else
			set_pc(exec.upc);

	} while(gflags.stepping ? getl() != 'q' : 1);

	normal_terminal();

	return 0;
}

