#include "src/components/mem.h"
#include "src/components/rom.h"
#include "src/components/ram.h"
#include "src/components/reg.h"
#include "src/components/decode.h"
#include "src/components/exec.h"
#include "src/display.h"
#include "src/emulator.h"
#include "src/types.h"
#include "src/term.h"
#include "src/utils.h"
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[]){
	// hide cursor and no echo for terminal
	hidden_terminal();
	// Listen to <C-c> (END OF THE PROGRAM) & take action
	init_end_sig();

	int c = ' ';
	int ppc;
	int bypass = 0;

	int key_input = 0;
	int input_value = 0;

	GFLAGS gflags;
	FETCH fetch;
	DECODE dcd;
	EXEC exec;

	// OSCCAL;
	

	// Update flags
	update_gflags(&gflags, argc, argv);

	if(gflags.pload == STATE_LOAD){
		int tmp;
		tmp = load_cpu_state(&gflags);
		if(tmp < 0){
			lprt(1, "Invalid cpu state file!");
		} else {
			set_pc(tmp);
		}
	}

	rom_init(gflags.program);

	if(gflags.pload == PROGRAM_LOAD){
		reg_init();
		ram_init();
	}

	do {

		if(c == 's'){
			dprt(term_size().x - 10, 2, "   [00FF00][bl]Saved!");
			save_cpu_state(gflags, ppc);
		}

		if(c == 'r'){
			reset_cpu();
		} else if(c != ' '){
			continue;
		}


		if(gflags.stepping == 0){

			if(getc_keep() == ' '){
				gflags.is_pause = ~gflags.is_pause;
			}

			if(getc_keep() == 'r' && gflags.is_sleep){
				reset_cpu();
			}


			if(getc_keep() == 's'){
				dprt(term_size().x - 10, 2, "   [00FF00]Saved!");
				save_cpu_state(gflags, ppc);
			}

			if(gflags.is_pause != 0){
				usleep(gflags.frequency);
				dprt(term_size().x - 10, 2, "[26aF9a][bl]❚❚ Paused");
				fflush(NULL);
				continue;
			}
		}

		// Capture data for each round
		fetch = rom_fetch();
		dcd = decode_inst(fetch.data);
		exec = soft_execute(dcd);

		// Display CPU
		emulate_cpu(&dcd, &gflags, input_value);

		if((gflags.is_sleep = exec.sleep) == 0){
			ppc = get_pc();

			// Update PC
			if(exec.upc == get_pc()){
				increment_pc();
			} else {
				set_pc(exec.upc);
			}

			bypass = execute(dcd); // Update Reg & Ram

			if(INPUT_EN){
				if((key_input = get_key()) >= 0){
					// set_sfr(&reg, GPIO_REGISTER, ~(key_input | get_w_reg()));
					set_sfr(GPIO_REGISTER, (key_input & ~get_w_reg()));
					input_value = (key_input & ~get_w_reg());
					// (i & ~w)
					// c = ~(a & b);
				}
			}

			// Update Register
			clear_sfr_bit(STATUS_REGISTER, 4);
			set_sfr_bit(STATUS_REGISTER, 6);
			if(gpio_temp != get_sfr(GPIO_REGISTER)){
				gpio_temp = get_sfr(GPIO_REGISTER);
				set_sfr_bit(STATUS_REGISTER, 7);
			}

		} else {
			dprt(term_size().x - 6, 2, "[26aF9a][bl]Sleep");
			set_sfr_bit(STATUS_REGISTER, 4);
		}

		// Interruption
		if(gflags.stepping == 0){
			usleep(gflags.frequency);
		}


		if(bypass){
			bypass = 0;
			set_pc(get_pc() + 1);
			continue;
		}

	} while(gflags.stepping ? (c = getl()) != 'q' : 1);


	// turn the echo on and make cursor visible
	normal_terminal();
	return 0;
}

