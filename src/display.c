#include "strfy.h"
#include "rules.h"
#include "term.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>


/* Update colors and return in (char *) for foreground: [FFFFFF] & for background: {FFFFFF} */
char *update_color(char *format, int clean){
	// increase the format size and use 'src'
	char *src = malloc(MALL * sizeof(char));
	strcpy(src, format);

	size_t src_len = strlen(src);
	char *tmp = malloc(src_len * 2 + 1); // Allocate temporary buffer
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
			if (sscanf(color_code, "%2x%2x%2x", &r, &g, &b) == 3) {
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
	free(tmp);

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
	return src;
}


/* goto given x, y coord (set cursor to [x, y])*/
void gotoxy(int x, int y){ 
	printf("\033[%d;%df", y, x); 
}

char *sgotoxy(int x, int y){ 
	char *s = malloc(100 * sizeof(char));
	sprintf(s, "\033[%d;%df", y, x); 
	return s;
}

/* print given char * in given x, y coord */
void printfxy(char *s, int x, int y){
	gotoxy(x, y);
	printf("%s", s);
}

// Decorative printf
void pprt(char *frmt, ...){
	char buff[MALL];
	va_list args;
	va_start(args, frmt);
	vsprintf(buff, frmt, args);
	printf("%s", update_color(buff, 1));
	fflush(NULL);
	va_end(args);
}


// Decorative printf (log pring (exit the program))
void lprt(int ecode, char *frmt, ...){
	normal_terminal();
	char buff[MALL];
	va_list args;
	va_start(args, frmt);
	vsprintf(buff, frmt, args);
	printf("%s\n", update_color(buff, 1));
	fflush(NULL);
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
	printf("%s", update_color(buff, 1));
	fflush(NULL);
	va_end(args);
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
	printf("%s", update_color("[808080]", 0));
	draw_line(x, y, height, VERTICAL, "╭", "╰");
	draw_line(x + widht, y, height, VERTICAL, "╮", "╯");
	draw_line(x + 1, y, widht, HORIZONTAL, "─", "─");
	draw_line(x + 1, y + height - 2, widht, HORIZONTAL, "─", "─");
	if(strcmp(title, "") != 0){
		printf("%s", update_color("[F0F0F0]", 0));
		dprt(x + 2, y, " %s ", title);
	}
}

