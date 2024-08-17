#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"


/* decimal_to_binary: convert given decimal to binary (char *) */
char *decimal_to_binary(int decimal_num) {
	int len = 14;
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




void update_gflags(GFLAGS *gflags, int argc, char *argv[]){
	gflags->step_mode = 0;
	int i;
	for(i = 0; i < argc; ++i){
		for(int j = 0; j < (int)strlen(argv[i]); ++j){
			if(argv[i][0] == '-'){
				switch(argv[i][j]){
					case 's':
						gflags->step_mode = 1;
						break;
				}
			}
		}
	}
}



/************************** TERMIOS **************************/

static struct termios old, current;

void init_term(){                /* terminal i/o settings */
	tcgetattr(0, &old);               /* grab old terminal i/o settings */
	current = old;                    /* make new settings same as old settings */
	current.c_lflag &= ~ICANON;       /* disable buffered i/o */
	current.c_lflag &= ~ECHO;         /* set no echo mode */
	tcsetattr(0, TCSANOW, &current);  /* use these new terminal i/o settings now */
}

// /* Restore old terminal i/o settings */
// void reset_termios(void){
// 	tcsetattr(0, TCSANOW, &old);
// }

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



