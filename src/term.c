#include "types.h"
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// show cursor
void enable_cursor(void){ printf("\e[?25h"); fflush(NULL); }
// hide cursor
void disable_cursor(void){ printf("\e[?25l"); fflush(NULL); }


static struct termios old, current;

// Disable Echo
void init_term(){                   // terminal i/o settings
	tcgetattr(0, &old);               // grab old terminal i/o settings
	current = old;                    // make new settings same as old settings
	current.c_lflag &= ~ICANON;       // disable buffered i/o
	current.c_lflag &= ~ECHO;         // set no echo mode
	tcsetattr(0, TCSANOW, &current);  // use these new terminal i/o settings now
}

// Enable Echo
void nrm_term(){
	tcgetattr(0, &old);
	current = old;
	current.c_lflag |= ICANON;
	current.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &current);
}

// Get a char without echo
char getl(void){
	char ch;
	init_term();
	ch = getchar();
	nrm_term();
	return ch;
}


/* ter_size: get terminal size [x, y] */
TERSIZ term_size(void){
	TERSIZ siz = {0, 0};
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	siz.x = w.ws_col;
	siz.y = w.ws_row;
	return siz;
}


/* clear terminal */
void cls_term(void){
#ifdef linux
	// printf("\e[3J\033c");
	system("clear");
#else
	printf("\e[1;1H\e[2J");
#endif
}

/* turn the echo on and make cursor visible */
void normal_terminal(void){
	cls_term();
	enable_cursor();
	nrm_term();
	fflush(NULL);
}


/* Hide terminal cursor and no echo */
void hidden_terminal(void){
	disable_cursor();  // Hide cursor block/line from the terminal
	init_term();       // No ouput from cursor
	fflush(NULL);
}
