#include "components/rom.h"
#include "structs.h"
#include "utils.h"
#include <stdio.h>


static TARLET tar_buff[5000];
static int tar_idx = 0;

/* prtprt: formatted print in (color-full) */
void pprt(int x, int y, char *frmt, ...) {
	char buff[MALL];
	va_list args;
	va_start(args, frmt);
	vsprintf(buff, frmt, args);
	// pprt(10, 10, "[red]Awesome");
	str_replace(buff, "[red]", "\x0A");
	str_replace(buff, "[grn]", "\x0B");
	str_replace(buff, "[yel]", "\x0C");
	str_replace(buff, "[blu]", "\x0D");
	str_replace(buff, "[nrm]", "\x0E");
	sprintf(buff, "%s%s", buff, "\x0F");
	va_end(args);
	
	for(int i = 0; i < (int)strlen(buff); ++i)
		tar_buff[tar_idx++] = (TARLET){ x + i, y, buff[i]};
}



int is_b_contains(int x, int y){
	int i;
	for(i = 0; i < tar_idx; ++i){
		if(tar_buff[i].x == x && tar_buff[i].y == y)
			return tar_buff[i].c;
	}
	return 0;
}


void emulate_cpu(){
	TERSIZ ts = term_size();
	ts.y -= 3;


	int x, y, c; x = y = c = 0;
	int xm, ym = 0; xm = ts.x - 1; ym = ts.y - 1;

	int s = 0;
	cls_term();
	// printf("[%d, %d] => %d\n", ts.x, ts.y, ts.x * ts.y);

	for(y = 0; y < ts.y; ++y){
		for(x = 0; x < ts.x; ++x, c = 0){


			if((c = is_b_contains(x, y)) != 0){
					printf("%c", c);

			} else {

					if(y == 0 && x == 0)
						printf("╭");
					else if (y == 0 && x == xm)
						printf("╮");
					else if(y == ym && x == 0)
						printf("╰");
					else if (y == ym && x == xm)
						printf("╯\n");
					else {
						if(x == 0)
							printf("│");
						else if(x == xm)
							printf("│\n");
						else if(y == 0)
							printf("─");
						else if(y == ym)
							printf("─");
						else
							printf(" ");
					}

			}

		}
	}
}

