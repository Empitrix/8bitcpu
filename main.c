#include "src/components/rom.h"
#include "src/components/decode.h"
#include "src/components/exec.h"
#include "src/structs.h"
#include "src/display.h"
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[]){

	GFLAGS gflags;
	FETCH fetch;
	DECODE dcd;

	ROM rom = rom_init();

	update_gflags(&gflags, argc, argv);


	while(gflags.stepping ? getl() != 'q' : 1){

		fetch = rom_fetch(rom);

		dcd = decode_run(fetch);

		// printf("BIN: %s - ", decimal_to_binary(fetch.data, 12));

		// execute_run(dcd);

		// pprt(10, 10, "[red]Awesome[nrm]");
		pprt(10, 10, "[red]Awesome");

		emulate_cpu();

		if(gflags.stepping == 0)
			usleep(500000);  // 500ms
	}

	return 0;
}

