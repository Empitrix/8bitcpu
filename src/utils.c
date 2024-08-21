#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "structs.h"
#include "strfy.h"
#include "display.h"



/* decimal_to_binary: convert given decimal to binary (char *) */
char *decimal_to_binary(int decimal_num, int len) {
	len = len + 2;
	char *binary_str = malloc(len + 1);

	// Fill with '0'
	memset(binary_str, '0', len);
	binary_str[len] = '\0';

	// Add '0b' prefix
	binary_str[0] = '0';
	binary_str[1] = 'b';

	// Convert decimal to binary and fill from the end
	int index = len - 1;
	while (decimal_num > 0 && index > 1) {
		binary_str[index] = (decimal_num % 2) + '0';
		decimal_num /= 2;
		index--;
	}

	return binary_str;
}


/* (Decimal TO Binary) convert decimal to 12 letter binary with colors */
char *dtob(int decimal){
	int len = 12;
	char *buff = malloc(MALL * sizeof(char));
	char *out = malloc(MALL * sizeof(char));
	strcpy(buff, decimal_to_binary(decimal, len));
	sprintf(out, "%s%s",        update_color("[459395]", 0), str_slice(buff, 0, 6));
	sprintf(out, "%s%s%s", out, update_color("[EB7C69]", 0), str_slice(buff, 6, 9));
	sprintf(out, "%s%s%s", out, update_color("[FCA637]", 0), str_slice(buff, 9, 14));
	// sprintf(out, "%s%s ", out, update_color("[FF0000]", 0));
	return out;
}

/* (Decimal to Hex) */
char *dtoh(int decimal) {
	char *hex = (char*)malloc(6); // Allocate memory for the hex string
	if (decimal < 0 || decimal > 4095) {
		hex[0] = 'E'; // Or handle the error differently
		hex[1] = 'R';
		hex[2] = 'R';
		hex[3] = '\0';
		return hex;
	}

	hex[0] = '0';
	hex[1] = 'x';
	hex[5] = '\0'; // Null terminator

	for (int i = 4; i >= 2; i--) {
		int digit = decimal % 16;
		decimal /= 16;
		hex[i] = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
	}

	return hex;
}



void update_gflags(GFLAGS *gflags, int argc, char *argv[]){
	gflags->stepping = 0;
	int i;
	for(i = 0; i < argc; ++i){
		for(int j = 0; j < (int)strlen(argv[i]); ++j){
			if(argv[i][0] == '-'){
				switch(argv[i][j]){
					case 's':
						gflags->stepping = 1;
						break;
				}
			}
		}
	}
}



/************************** Terminal **************************/

static struct termios old, current;

void init_term(){                   // terminal i/o settings
	tcgetattr(0, &old);               // grab old terminal i/o settings
	current = old;                    // make new settings same as old settings
	current.c_lflag &= ~ICANON;       // disable buffered i/o
	current.c_lflag &= ~ECHO;         // set no echo mode
	tcsetattr(0, TCSANOW, &current);  // use these new terminal i/o settings now
}

void nrm_term(){
	tcgetattr(0, &old);
	current = old;
	current.c_lflag |= ICANON;
	current.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &current);
}

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


void cls_term(void){
#ifdef linux
	// printf("\e[3J\033c");
	system("clear");
#else
	printf("\e[1;1H\e[2J");
#endif
}



struct sigaction old_action;

void sigint_handler(int sig_no){
	cls_term();
	enable_cursor();
	nrm_term();
	sigaction(SIGINT, &old_action, NULL);
	kill(0, SIGINT);
}


void init_end_sig(){
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = &sigint_handler;
	sigaction(SIGINT, &action, &old_action);
}

