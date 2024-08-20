#include "utils.h"




void update_color(char buff[]){
	str_replace(buff, "[nrm]", "\033[39m");
	str_replace(buff, "[blk]", "\033[30m");
	str_replace(buff, "[drd]", "\033[31m");
	str_replace(buff, "[dgn]", "\033[32m");
	str_replace(buff, "[yel]", "\033[33m");
	str_replace(buff, "[dlu]", "\033[34m");
	str_replace(buff, "[dmg]", "\033[35m");
	str_replace(buff, "[dcn]", "\033[36m");
	str_replace(buff, "[lgr]", "\033[37m");
	str_replace(buff, "[dgr]", "\033[90m");
	str_replace(buff, "[red]", "\033[91m");
	str_replace(buff, "[grn]", "\033[92m");
	str_replace(buff, "[org]", "\033[93m");
	str_replace(buff, "[blu]", "\033[94m");
	str_replace(buff, "[mgn]", "\033[95m");
	str_replace(buff, "[cyn]", "\033[96m");
	str_replace(buff, "[wht]", "\033[97m");

	str_replace(buff, "[nrm_b]", "\033[49m");
	str_replace(buff, "[blk_b]", "\033[40m");
	str_replace(buff, "[drd_b]", "\033[41m");
	str_replace(buff, "[dgn_b]", "\033[42m");
	str_replace(buff, "[yel_b]", "\033[43m");
	str_replace(buff, "[dlu_b]", "\033[44m");
	str_replace(buff, "[dmg_b]", "\033[45m");
	str_replace(buff, "[dcn_b]", "\033[46m");
	str_replace(buff, "[lgr_b]", "\033[47m");
	str_replace(buff, "[dgr_b]", "\033[100m");
	str_replace(buff, "[red_b]", "\033[101m");
	str_replace(buff, "[grn_b]", "\033[101m");
	str_replace(buff, "[org_b]", "\033[103m");
	str_replace(buff, "[blu_b]", "\033[104m");
	str_replace(buff, "[mgn_b]", "\033[105m");
	str_replace(buff, "[cyn_b]", "\033[106m");
	str_replace(buff, "[wht_b]", "\033[107m");
}





void gotoxy(int x, int y){ 
	printf("\033[%d;%df", y, x); 
}

void printfxy(char *s, int x, int y){
	gotoxy(x, y);
	printf("%s", s);
}


/* dprt: Display Print */
void dprt(int x, int y, char *frmt, ...) {
	char buff[MALL];
	va_list args;
	va_start(args, frmt);
	vsprintf(buff, frmt, args);
	update_color(buff);
	gotoxy(x, y);
	printf("%s%s%s", buff, "\033[39m", "\033[49m");
	fflush(NULL);
	va_end(args);
}


typedef enum LINE_TYPE {
	HORIZONTAL,
	VERTICAL,
} LINE_TYPE;


void draw_line(int x, int y, int size, LINE_TYPE lt, char *a, char *b, int light){ 
	int i;
	for(i = 0 ; i < size; ++i){

		if(i == 0)
			printfxy(a, x, y);
		else if (i == size - 1)
			printfxy(b, x, y);


		else {
			if(lt == HORIZONTAL){
				x++;
				printfxy(light ? "┄": "─", x, y);
			} else {
				y++;
				printfxy(light ? "┆": "│", x, y);
			}

		}

	}
}


void draw_box(int x, int y, int widht, int height){
	draw_line(x, y, height, VERTICAL, "╭", "╰", 0);
	draw_line(x + widht, y, height, VERTICAL, "╮", "╯", 0);
	draw_line(x + 1, y, widht, HORIZONTAL, "─", "─", 0);
	draw_line(x + 1, y + height - 2, widht, HORIZONTAL, "─", "─", 0);
	// ╭─╮
	// │ │
	// ╰─╯
}


void scrollable(){
}

