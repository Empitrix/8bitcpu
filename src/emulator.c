#include "structs.h"
#include "utils.h"
#include "display.h"


void prtfrm(TERSIZ ts){
	int x, y;
	int xm, ym;
	xm = ts.x - 1;
	ym = ts.y - 1;

	for(y = 1; y < ts.y; ++y)
		for(x = 1; x < ts.x; ++x)

			if(x == 1 && y == 1)
				printfxy("╭", x, y);
			else if (x == 1 && y == ym)
				printfxy("╮", x, y);
			else if(x == xm && y == 1)
				printfxy("╰", x, y);
			else if (x == xm && y == ym)
				printfxy("╯", x, y);
			else {
				if(y == 1 || y == ym)
					printfxy("│", x, y);
				else if(x == 1 || x == xm)
					printfxy("─", x, y);
				else
					printfxy(" ", x, y);
			}
}


void emulate_cpu(){
	TERSIZ ts = term_size(); ts.y += 1;

	cls_term();
	prtfrm(ts);
}

