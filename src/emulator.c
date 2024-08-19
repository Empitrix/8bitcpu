#include "components/rom.h"
#include "components/exec.h"
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


void emulate_cpu(ROM rom, EXEC exec){
	TERSIZ ts = term_size(); ts.y += 1;

	cls_term();
	prtfrm(ts);




	dprt(10, 0, " [blu]8-BIT CPU[nrm] ");
	draw_box(2, 2, 34, ts.y / 2);

	// draw_line(30, 2, (ts.y / 2), VERTICAL, "┬", "┴", 1);
	dprt(4, 2, " ROM ");



	dprt(3, 9, "PC: [red]%d", get_pc());
	// for(int i = 0; i < (ts.y / 2) - 3; ++i){
	// 	if(i == get_pc())
	// 		dprt(3, 3 + i, "[wht_b][blk]%-4d [yel_b][blk]%s[nrm]", i, decimal_to_binary(rom.mcode[i], 12));
	// 	else
	// 		dprt(3, 3 + i, "[blk_b][wht]%-4d %s[nrm]", i, decimal_to_binary(rom.mcode[i], 12));
	// }

	for(int i = 0; i < (ts.y / 2) - 3; ++i){
		if(i == get_pc())
			dprt(3, 3 + i, "[wht_b][blk]%-4d [yel_b][blk]%s[nrm][nrm_b] - [blu_b][blk]%s[nrm][nrm_b] %s", i, dtob(rom.mcode[i]), dtoh(rom.mcode[i]), execute_info(rom.mcode[i]) );
		else
			dprt(3, 3 + i, "[blk_b][wht]%-4d %s[nrm]", i, dtob(rom.mcode[i]));
	}

	dprt(ts.x, ts.y, "");

}


