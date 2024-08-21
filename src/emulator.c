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


void emulate_cpu(ROM rom, EXEC exec){
	TERSIZ ts = term_size(); ts.y += 1;

	cls_term();

	// printf("\e[?25l"); // hide cursor

	prtfrm(ts);
	dprt(10, 0, " [3153eb]8-BIT CPU[{}] ");  // header





	// draw_line(30, 2, (ts.y / 2), VERTICAL, "┬", "┴", 1);

	// ROM box
	draw_box(2, 3, 34, ts.y / 2);
	dprt(4, 3, " ROM ");

	draw_box(37, 3, ts.x - 38, ts.y / 1.5);
	dprt(39, 3, " Expandable ");


	dprt(2, 2, "{FFFFFF}[000000] PC: %d | %*s ", get_pc(), ts.x - 12, "STATUS LINE");


	// for(int i = 0; i < (ts.y / 2) - 3; ++i){
	// 	if(i == get_pc())
	// 		dprt(3, 4 + i, "[wht_b][blk]%-4d [yel_b][blk]%s[nrm][nrm_b] - [blu_b][blk]%s[nrm][nrm_b] %s", i, dtob(rom.mcode[i]), dtoh(rom.mcode[i]), execute_info(rom.mcode[i]) );
	// 	else
	// 		dprt(3, 4 + i, "[blk_b][wht]%-4d %s[nrm]", i, dtob(rom.mcode[i]));
	// }

	int max_h = (ts.y / 2) - 3;

	int linen = 0;

	if(get_pc() > max_h / 2){
		linen = (fake_pc % RAMSIZ) - (max_h / 2);
		if(linen + (max_h / 2) + (max_h / 2) + 2 > RAMSIZ)
			linen = RAMSIZ - max_h;
	}

	for(int i = 0; i < max_h; ++i){
		if((linen + i)  == get_pc())
			dprt(3, 4 + i, "[fcd200]%-4d [u][000000]%s[{}] - [2979FF]%s[{}] %s",
					i, dtob(rom.mcode[i]), dtoh(rom.mcode[i]), execute_info(rom.mcode[i]));
		else
			dprt(3, 4 + i, "[808080]%-4d [{}]%s", i, dtob(rom.mcode[i]));
	}



}


