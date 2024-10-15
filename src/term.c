#include "types.h"
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// show cursor
void enable_cursor(void){
	printf("\033[?25h");
	fflush(NULL);
}

// hide cursor
void disable_cursor(void){
	printf("\033[?25l");
	fflush(NULL);
}


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


/* clear terminal */
void cls_term(void){
#ifdef linux
	printf("\033[H");
#else
	printf("\033[1;1H\033[2J");
#endif
}


// static int tmp_y_siz = 0;
// static int tmp_x_siz = 0;

/* ter_size: get terminal size [x, y] */
TERSIZ term_size(void){
	TERSIZ siz = {0, 0};
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	siz.x = w.ws_col;
	siz.y = w.ws_row;

	// // Clear terminal if user changes the window
	// if(siz.x != tmp_x_siz || siz.y != tmp_y_siz){ system("clear"); }
	// tmp_y_siz = siz.y;
	// tmp_x_siz = siz.x;

	return siz;
}

/* turn the echo on and make cursor visible */
void normal_terminal(void){
	system("clear");
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

