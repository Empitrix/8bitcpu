#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "structs.h"
#include "strfy.h"
#include "term.h"
#include "display.h"



/* Decimal TO Binary: convert given decimal to binary (char *) */
char *dtob(int decimal_num, int len) {
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


/* Return Binary LED bar */
char *dtob_led(int num, int len) {
	char *buff = malloc(MALL * sizeof(char));
	buff = dtob(num, len);
	sprintf(buff, "%s", str_slice(buff, 2, len + 2));
	str_replace(buff, "0", "[909090]❚");
	str_replace(buff, "1", "[00FF00]❚");
	return buff;
}


/* (Decimal TO Binary) convert decimal to 12 letter binary with colors */
char *dtob3sec(int decimal, char *a, char *b, char *c){
	int len = 12;
	char *buff = malloc(MALL * sizeof(char));
	char *out = malloc(MALL * sizeof(char));
	strcpy(buff, dtob(decimal, len));
	sprintf(out, "%s%s",        update_color(a, 0), str_slice(buff, 0, 6));
	sprintf(out, "%s%s%s", out, update_color(b, 0), str_slice(buff, 6, 9));
	sprintf(out, "%s%s%s", out, update_color(c, 0), str_slice(buff, 9, 14));
	return out;
}


/* decimal to binary for 2 section */
char *dtob2sec(int decimal, char *a, char *b){
	int len = 12;
	char *buff = malloc(MALL * sizeof(char));
	char *out = malloc(MALL * sizeof(char));
	strcpy(buff, dtob(decimal, len));
	sprintf(out, "%s%s",        update_color(a, 0), str_slice(buff, 0, 5));
	sprintf(out, "%s%s%s", out, update_color(b, 0), str_slice(buff, 5, 14));
	return out;
}


/* dtoh: (Decimal TO Hex) converts given decimal into hex string with size of 'siz' */
char *dtoh(int decimal, int siz){
	char *hex = (char*)malloc(siz + 3);
	hex[0] = '0';
	hex[1] = 'x';
	hex[siz + 3] = '\0';
	for(int i = siz + 1; i >= 2; i--){
		int digit = decimal % 16;
		decimal /= 16;
		hex[i] = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
	}
	return hex;
}


/* update_gflags: Update Global Flags */
void update_gflags(GFLAGS *gflags, int argc, char *argv[]){
	gflags->stepping = gflags->is_pause = 0;
	gflags->frequency = 500000;
	memset(gflags->program, '\0', MALL);
	int ps, fs = 0; // program save
	int i;
	for(i = 0; i < argc; ++i){

		if(ps == 1){
			strcpy(gflags->program, argv[i]);
			ps = 0;
			continue;
		}
		
		if(fs == 1){
			gflags->frequency = atoi(argv[i]);

			if(gflags->frequency == 0)
				gflags->frequency = 500000;
			else if (gflags->frequency > 1000000)
				gflags->frequency = 1;
			else
				gflags->frequency = 1000000 / gflags->frequency;

			fs = 0;
			continue;
		}

		for(int j = 0; j < (int)strlen(argv[i]); ++j){
			if(argv[i][0] == '-'){
				switch(argv[i][j]){
					case 's':
						gflags->stepping = 1;
						break;
					case 'p':
						ps = 1;
						break;
					case 'f':
						fs = 1;
						break;
					default:
						break;
				}
			}
		}

	}

	if(ps == 1)
		lprt(1, "After [afaf00]'-p'[ffffff] you should put the path to the supported [f44336].bin[ffffff] file!");

	if(fs == 1)
		lprt(1, "After [afaf00]'-f'[ffffff] you should put clock frequency!");

	if(strcmp(gflags->program, "") == 0)
		lprt(1, "By using [afaf00]'-p <path>'[ffffff] specify the path to the program file!");
}



/************************** Signals **************************/
struct sigaction old_action;

// Action for end of the program signal <C-c>
void end_sig_func(int sig_no){
	normal_terminal();
	sigaction(SIGINT, &old_action, NULL);
	kill(0, SIGINT);
}

// capture end of program signal <C-c>
void init_end_sig(){
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = &end_sig_func;
	sigaction(SIGINT, &action, &old_action);
}


/* get use char with no interruption */
int getc_keep(void){
	fd_set readfds;
	struct timeval tv;
	char ch;

	// Set stdin to non-blocking mode
	int flags = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0; // Check immediately

	int ready = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
	if (ready == 1){
		read(STDIN_FILENO, &ch, 1);
		return ch;
	}

	return -1;
}

