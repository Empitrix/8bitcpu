#include "components/rom.h"
#include "components/exec.h"
#include "rules.h"
#include "structs.h"
#include "utils.h"
#include "display.h"


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


void emulate_cpu(ROM rom, DECODE dcd, EXEC exec){
	TERSIZ ts = term_size(); ts.y += 1;
	cls_term();

	prtfrm(ts);
	dprt(10, 0, " [3153eb]8-BIT CPU[{}] ");  // header

	int rom_p_s = 50; 
	// ROM box
	draw_box(2, 3, rom_p_s, ts.y / 2);
	dprt(4, 3, " ROM ");



	dprt(2, 2, "{FFFFFF}[000000] PC: %d | %*s ", get_pc(), ts.x - 12, "STATUS LINE");


	// ROM
	int max_h = (ts.y / 2) - 3;
	int linen = 0;

	if(get_pc() > max_h / 2){
		linen = (fake_pc % RAMSIZ) - (max_h / 2);
		if(linen + (max_h / 2) + (max_h / 2) + 2 > RAMSIZ)
			linen = RAMSIZ - max_h;
	}


	for(int i = 0; i < max_h; ++i){
		if((linen + i)  == get_pc())
			dprt(3, 4 + i, "%s%-4s [{}][u]%s", i == 0 ? "[F44336]" : "[fcd200]", dtoh(i, 4), exec_info(rom.mcode[i]));
		else
			dprt(3, 4 + i, "%s%-4s [{}]%s", i == 0 ? "[D32F2F]" : "[808080]", dtoh(i, 4), exec_info(rom.mcode[i]));
	}


	// RAM
	draw_box(rom_p_s + 3, 3, ts.x - rom_p_s - 4, ts.y / 1.5);
	dprt(rom_p_s + 5, 3, " Expandable ");


}


