#include "src/components/rom.h"
#include "src/structs.h"
#include <unistd.h>


int main(int argc, char *argv[]){

	GFLAGS gflags;
	FETCH fetch;
	ROM rom = rom_init();

	update_gflags(&gflags, argc, argv);
	// dump_rom(rom);


	while(gflags.step_mode ? getl() != 'q' : 1){

		fetch = rom_fetch(rom);


		printf("%d\n", fetch.data);
		// set_pc(5);

		if(gflags.step_mode == 0)
			usleep(500000);  // 500ms
	}

	return 0;
}

