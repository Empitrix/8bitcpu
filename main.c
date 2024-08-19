#include "src/components/rom.h"
#include "src/components/decode.h"
#include "src/components/exec.h"
#include "src/structs.h"
#include "src/emulator.h"
#include "src/utils.h"
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

		execute_run(dcd);

		emulate_cpu();
		
		dprt(10, 0, " [blu]CPU[nrm] ");


		dprt(3, 9, "PC: [red]%d", get_pc());
		for(int i = 0; i < 5; ++i){
			if(i == get_pc())
				dprt(3, 10 + i, "[yel_b][blk]%d %s[nrm]", i, decimal_to_binary(rom.mcode[i], 12));
			else
				dprt(3, 10 + i, "[blk_b][wht]%d %s[nrm]", i, decimal_to_binary(rom.mcode[i], 12));
		}


		if(gflags.stepping == 0)
			usleep(500000);  // 500ms
	}

	return 0;
}

