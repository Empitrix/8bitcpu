#include "components/rom.h"
#include "components/exec.h"
#include "rules.h"
#include "structs.h"
#include "utils.h"
#include "display.h"
#include <stdio.h>


void prtfrm(TERSIZ ts){
	int x, y;
	int xm, ym;

	for(y = 1; y < ts.y + 1; ++y)
		for(x = 1; x < ts.x + 1; ++x)

			if(y == 1 && x == 1)
				printfxy("╭", x, y);
			else if (y == 1 && x == ts.x)
				printfxy("╮", x, y);
			else if(y == ts.y && x == 1)
				printfxy("╰", x, y);
			else if (y == ts.y && x == ts.x)
				printfxy("╯", x, y);
			else {
				if(x == 1 || x == ts.x)
					printfxy("│", x, y);
				else if(y == 1 || y == ts.y)
					printfxy("─", x, y);
				else
					printfxy(" ", x, y);
			}
}


static int fake_pc = 110;


void emulate_cpu(ROM rom, DECODE dcd, EXEC exec, REG reg, RAM ram){
	TERSIZ ts = term_size(); ts.y += 1;
	cls_term();

	prtfrm(ts);
	dprt(10, 0, " [3153eb]8-BIT CPU[{}] ");  // header

	int rom_p_s = (ts.x / 2) - 1;  // ROM Pannel Size

	// ROM Pannel
	draw_box(2, 3, rom_p_s, ts.y - 3);
	dprt(4, 3, " ROM ");

	// Registers Pannel
	draw_box(rom_p_s + 3, 3, ts.x - rom_p_s - 4, 13); // ts.y / 2 - 4
	dprt(rom_p_s + 5, 3, " Registers ");

	// RAM Pannel
	draw_box((ts.x / 2) + 2, ts.y / 2 - 2, (ts.x / 2) - 2, ts.y / 2 + 2);
	dprt((ts.x / 2) + 4, ts.y / 2 - 2, " RAM ");


	dprt(2, 2, "{AED0FB}[000000] PC: %d │ GPIO 6: %s │ %*s ", get_pc(), dtoh(reg.registers[6], 2), ts.x - 27, "STATUS LINE");


	// ROM Pannel
	// int max_h = (ts.y / 2) - 3;
	int max_h = (ts.y) - 6;
	int linen = 0;

	if(get_pc() > max_h / 2){
		linen = (fake_pc % ROMSIZ) - (max_h / 2);
		if(linen + (max_h / 2) + (max_h / 2) + 2 > ROMSIZ)
			linen = ROMSIZ - max_h;
	}


	// ROM list view
	for(int i = 0; i < max_h; ++i){
		if((linen + i)  == get_pc())
			dprt(4, 4 + i, "%s%-4s [{}][u]%s", i == 0 ? "[F44336]" : "[fcd200]", dtoh(i, 4), exec_info(rom.mcode[i]));
		else
			dprt(4, 4 + i, "%s%-4s [{}]%s", i == 0 ? "[D32F2F]" : "[808080]", dtoh(i, 4), exec_info(rom.mcode[i]));
	}



	// REG
	for(int i = 0; i < REGSIZ; ++i)
		dprt(ts.x / 2 + 4, 4 + i, "%-16s [FB8C00]%s [558B2F]%s", i == 6 ? "[FFFFFF]GPIO" : "[999999]RESEVERD", d_to_b(reg.registers[i], 8), dtoh(reg.registers[i], 2));

	// RAM
	for(int i = 0; i < (ts.y / 2 - 1); ++i)
		dprt((ts.x / 2) + 4, ((ts.y / 2) - 1) + i, "[999999]%-4s [FB8C00]%s [558B2F]%s", dtoh(i + 16, 2), d_to_b(ram.ram[i], 8), dtoh(ram.ram[i], 2));




	fflush(NULL);

}


