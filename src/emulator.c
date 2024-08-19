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


void emulate_cpu(){
	TERSIZ ts = term_size(); ts.y += 1;

	cls_term();
	prtfrm(ts);

	// draw_line(50, 20, 10, HORIZONTAL, "+", "+");

	draw_line(20, 20, 10, VERTICAL, "╭", "╰");
	draw_line(40, 20, 10, VERTICAL, "╮", "╯");
}


// ╭─╮
// │ │
// ╰─╯

