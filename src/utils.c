#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wchar.h>
#include "components/mem.h"
#include "rules.h"
#include "types.h"
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
	// char *buff = malloc(len * sizeof(char));
	char *buff = (char *)calloc(50, sizeof(char));
	buff = dtob(num, len);
	sprintf(buff, "%s", str_slice(buff, 2, len + 2));
	str_replace(buff, "0", "[909090]❚");
	str_replace(buff, "1", "[00FF00]❚");
	return buff;
}

/* dtoh: (Decimal TO Hex) converts given decimal into hex string with size of 'siz' */
char *dtoh(int decimal, int size) {
	char *hex = malloc(MALL * sizeof(char));
	if (hex == NULL) {
		return NULL;
	}

	hex[0] = '0';
	hex[1] = 'x';
	hex[size + 2] = '\0'; // Add null terminator

	// Handle padding
	for (int i = size + 1; i >= 2; i--) {
		if (decimal == 0 && i > 2) {
			hex[i] = '0';
		} else {
			int digit = decimal % 16;
			if (digit < 10) {
				hex[i] = '0' + digit;
			} else {
				hex[i] = 'A' + digit - 10;
			}
			decimal /= 16;
		}
	}
	return hex;
}


/* update_gflags: Update Global Flags */
void update_gflags(GFLAGS *gflags, int argc, char *argv[]){
	gflags->stepping = gflags->is_pause = 0;
	gflags->frequency = 500000;
	gflags->pload = PROGRAM_LOAD;
	gflags->is_sleep = 0;
	gflags->console_en = 0;
	memset(gflags->program, '\0', MALL);
	memset(gflags->load, '\0', MALL);
	int ps, fs, ls = 0; // program save
	int i;
	for(i = 0; i < argc; ++i){

		if(ps == 1){
			strcpy(gflags->program, argv[i]);
			ps = 0;
			continue;
		}
		
		if(ls == 1){
			strcpy(gflags->load, argv[i]);
			ls = 0;
			gflags->pload = STATE_LOAD;
			continue;
		}

		if(fs == 1){
			gflags->frequency = atoi(argv[i]);

			if(gflags->frequency == 0){
				gflags->frequency = 500000;
			} else if (gflags->frequency > 1000000){
				gflags->frequency = 1;
			} else{
				gflags->frequency = 1000000 / gflags->frequency;
			}

			fs = 0;
			continue;
		}

		for(int j = 0; j < (int)strlen(argv[i]); ++j){
			if(argv[i][0] == '-'){
				switch(argv[i][j]){
					case 's':
						gflags->stepping = 1;
						break;
					case 'c':
						gflags->console_en = 1;
						break;
					case 'p':
						ps = 1;
						break;
					case 'f':
						fs = 1;
						break;
					case 'l':
						ls = 1;
						break;
					default:
						break;
				}
			}
		}

	}

	if(ps == 1){
		lprt(1, "After [afaf00]'-p'[ffffff] you should put the path to the supported [f44336].bin[ffffff] file!");
	}

	if(fs == 1){
		lprt(1, "After [afaf00]'-f'[ffffff] you should put clock frequency!");
	}

	if(ls == 1){
		lprt(1, "After [afaf00]'-l'[ffffff] you should put save state file!");
	}

	if(strcmp(gflags->program, "") == 0 && strcmp(gflags->load, "") == 0){
		lprt(1, "By using [afaf00]'-p <path>'[ffffff] specify the path to the program file!");
	}
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


/* save_cpu_state: save current state of CPU to a file */
void save_cpu_state(GFLAGS gf, int pc){
	FILE *fp;
	fp = fopen("./cpu_state.txt", "w+");
	if(fp == NULL)
		return;
	fprintf(fp, "%s\n", gf.program);
	fprintf(fp, "%d\n", gf.frequency);
	fprintf(fp, "%d\n", pc);
	int i;
	for(i = 0; i < REGSIZ; ++i){
		fprintf(fp, "%d\n", REGISTERS[i]);
	}
	for(i = 0; i < RAMSIZ; ++i){
		fprintf(fp, "%d\n", RAM[i]);
	}
	fclose(fp);
}


/* load_cpu_state: Load state of CPU from a file and return the PC */
int load_cpu_state(GFLAGS *gf) {
	FILE *fp;

	int pc = 0;

	fp = fopen(gf->load, "r");
	if(fp == NULL){
		return -1;
	}

	if (fscanf(fp, "%s\n%d\n%d\n", gf->program, &gf->frequency, &pc) != 3) {
		fprintf(stderr, "Error reading file: Invalid format\n");
		fclose(fp);
		return -1;
	}

	for (int i = 0; i < REGSIZ; ++i) {
		if (fscanf(fp, "%d\n", (int *)&REGISTERS[i]) != 1){
			fclose(fp);
			return -1;
		}
	}

	for (int i = 0; i < RAMSIZ; ++i) {
		if (fscanf(fp, "%d\n", (int *)&RAM[i]) != 1) {
			fclose(fp);
			return -1;
		}
	}

	fclose(fp);
	return pc;
}


/* Extract Decimal from binary (start and end are included)*/
int edfb(int decimal, int start, int end) {
	start--;
	end--;
	// Create a mask to extract the desired bits
	int mask = (1 << (end - start + 1)) - 1;

	// Shift the mask to the correct position
	mask <<= start;

	// Extract the desired bits
	int ebits = (decimal & mask) >> start;
	return ebits;
}



int any_use_bit(OPCODES op){
	switch (op) {
		case ANDLW_OP:
			return 1;
		case IORLW_OP:
			return 1;
		case RETLW_OP:
			return 1;
		case XORLW_OP:
			return 1;
		case MOVWF_OP:
			return 1;
		case MOVLW_OP:
			return 1;
		case CLRF_OP:
			return 1;
		default:
			return 0;
	}
	return 0;
}



int get_key(void){
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
	if (ready == 1) {
		read(STDIN_FILENO, &ch, 1);
		// return ch;  // if you want to return char

		ch = ch - '1';
		if(ch >= 0 && ch <= 7){
			return 0b00000000 | 1 << (ch);
		} else {
			return -1;
		}
	}
	return -1;
}




// char *binary_led(int num, int keynum) {
// 	char *binary = (char *)calloc(100, sizeof(char));
// 	int idx = 7;
// 	while (idx >= 0) {
// 		if(edfb(keynum, idx + 1, idx + 1) == 1){
// 			strcat(binary, "[FF0000]❚");
// 		} else {
// 			if((num & 1) == 1){
// 				strcat(binary, "[00FF00]❚");
// 			} else {
// 				strcat(binary, "[909090]❚");
// 			}
// 		}
// 		num >>= 1;
// 		idx--;
// 	}
// 	return binary;
// }

int is_input_on(int inpt, int idx){
	idx = idx % 8;
	int binary = (inpt & ~get_w_reg());
	return edfb(binary, idx + 1, idx + 1) == 1;
}


int is_bit_input(int idx){
	idx = idx % 8;
	return edfb(get_w_reg(), idx + 1, idx + 1) == 0;
}


char *binary_led(int num, uint8_t keynum) {
	char *binary = (char *)calloc(100, sizeof(char));
	int idx = 0;

	for (int i = 7; i >= 0; i--) {
		if(INPUT_EN && is_bit_input(i)){
			if(is_input_on(keynum, i)){
				strcat(binary, "[FF0000]❚");
			} else {
				strcat(binary, "[909090]❚");
			}
		} else {
			if (num & (1 << i)) {
				strcat(binary, "[00FF00]❚");
			} else {
				strcat(binary, "[909090]❚");
			}
		}

		/*
		if(edfb(keynum, idx + 1, idx + 1) == 1){
			strcat(binary, "[FF0000]❚");
		} else if(edfb(~keynum, idx + 1, idx + 1) == 1) {
			strcat(binary, "[00FF00]❚");
		} else {
			strcat(binary, "[909090]❚");
		}
		*/
	}
	return binary;
}

