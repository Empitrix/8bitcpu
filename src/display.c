#include "strfy.h"
#include "rules.h"
#include "term.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>


/* Update colors and return in (char *) for foreground: [FFFFFF] & for background: {FFFFFF} */
void update_color(char *format, int clean, char src[]){
	strcpy(src, format);

	char tmp[MAXSIZ]; // Allocate temporary buffer
	char *dst = tmp;
	char *ptr = src;


	while (*ptr) {
		if (*ptr == '[' || *ptr == '{') {
			// Check if it's a color code
			char color_code[7];
			strncpy(color_code, ptr + 1, 6);
			color_code[6] = '\0';

			// Convert hex color to RGB
			int r, g, b;
			if (sscanf(color_code, "%2x%2x%2x", (unsigned int *)&r, (unsigned int *)&g, (unsigned int *)&b) == 3) {
				if (*ptr == '[') {
					dst += sprintf(dst, "\033[38;2;%d;%d;%dm", r, g, b);  // fg
				} else if (*ptr == '{') {
					dst += sprintf(dst, "\033[48;2;%d;%d;%dm", r, g, b);  // bg
				}
				ptr += 8;  // Don't copy the color code
			} else {
				*dst++ = *ptr++;  // Just copy if color code is invalid
			}
		} else {
			*dst++ = *ptr++;  // Copy other characters
		}
	}

	*dst = '\0';

	// Copy the modified string back to the original buffer
	strncpy(src, tmp, dst - tmp);
	src[dst - tmp] = '\0';
	// free(tmp);

	// Insert reset codes at the end
	if(clean == 1){
		strcat(src, "\033[0m\033[49m");
	}

	str_replace(src, "[{}]", "\033[0m\033[49m");
	str_replace(src, "[]", "\033[0m");
	str_replace(src, "{}", "\033[49m");
	str_replace(src, "[u]", "\033[4m");
	str_replace(src, "[bl]", "\x1b[5m");
	str_replace(src, "[b]", "\x1b[1m");
	str_replace(src, "[i]", "\x1b[3m");
}


/* goto given x, y coord (set cursor to [x, y])*/
void gotoxy(int x, int y){ 
	printf("\033[%d;%df", y, x); 
}

void sgotoxy(int x, int y, char src[]){ 
	sprintf(src, "\033[%d;%df", y, x); 
}

/* print given char * in given x, y coord */
void printfxy(char *s, int x, int y){
	gotoxy(x, y);
	printf("%s", s);
}


// Decorative printf (pring & exit the program)
void lprt(int ecode, char *frmt, ...){
	normal_terminal();
	char buff[MALL];
	va_list args;
	va_start(args, frmt);
	vsprintf(buff, frmt, args);
	char colored[MAXSIZ];
	update_color(buff, 1, colored);
	printf("%s\n", colored);
	va_end(args);
	exit(ecode);
}


/* dprt: Decorative Print */
void dprt(int x, int y, char *frmt, ...) {
	char buff[MALL];
	va_list args;
	va_start(args, frmt);
	vsprintf(buff, frmt, args);
	gotoxy(x, y);
	char colored[MAXSIZ];
	update_color(buff, 1, colored);
	printf("%s", colored);
	fflush(NULL);
	va_end(args);
}


/* fixed_dprt: Decorative Print with right margin (width) */
void fixed_dprt(int x, int y, int width, char *frmt, ...) {
	char buff[MALL];
	va_list args;
	va_start(args, frmt);
	vsprintf(buff, frmt, args);
	gotoxy(x, y);
	va_end(args);

	char output[MAXSIZ];
	char colored[MAXSIZ];
	update_color(buff, 1, colored);
	sprintf(output, "%s", colored);
	int len = (int)strlen(output);
	printf("%-*s\n", len + width, output);
}


// Line type
typedef enum LINE_TYPE {
	HORIZONTAL,
	VERTICAL,
	HORIZONTAL_LIGHT,
	VERTICAL_LIGHT,
} LINE_TYPE;


/* Draw a line */
void draw_line(int x, int y, int size, LINE_TYPE lt, char *a, char *b){ 
	int i;
	for(i = 0 ; i < size; ++i){
		if(i == 0){
			printfxy(a, x, y);
		} else if (i == size - 1){
			printfxy(b, x, y);
		} else {
			if(lt == HORIZONTAL || lt == HORIZONTAL_LIGHT){
				x++;
				printfxy(lt == HORIZONTAL_LIGHT ? "┄": "─", x, y);
			} else {
				y++;
				printfxy(lt == VERTICAL_LIGHT ? "┆": "│", x, y);
			}
		}
	}
}


/* draw_box: draw a box && use given title if it's not <empty> */
void draw_box(int x, int y, int widht, int height, char *title){
	char colored[MAXSIZ];
	update_color("[808080]", 0, colored);
	int empty_title = strcmp(title, "");
	printf("%s", colored);
	draw_line(x, y, height, VERTICAL, "╭", "╰");  // Left
	draw_line(x + widht, y, height, VERTICAL, "╮", "╯");  // Right

	if(empty_title == 0){
		draw_line(x + 1, y, widht, HORIZONTAL, "─", "─");  // Top
	} else {
		draw_line(x + 1, y, 1, HORIZONTAL, "─", "─");  // Top
		draw_line(x + (int)strlen(title) + 4, y, widht - (int)strlen(title) - 3, HORIZONTAL, "─", "─");  // Top
	}

	draw_line(x + 1, y + height - 2, widht, HORIZONTAL, "─", "─");  // Bottom
	if(empty_title != 0){

		if(strcmp(title, "8-BIT CPU") == 0){
			update_color("[98C379]", 0, colored);
		} else {
			update_color("[F0F0F0]", 0, colored);
		}
		printf("%s", colored);
		dprt(x + 2, y, " %s ", title);
	}
}

