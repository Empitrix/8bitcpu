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
#include "term.h"
#include "display.h"



/* Decimal TO Binary: convert given decimal to binary (char *) */
void dtob(int decimal_num, int len, char binary_str[]) {
	len = len + 2;

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

	// return binary_str;
}


/* dtoh: (Decimal TO Hex) converts given decimal into hex string with size of 'siz' */
char *dtoh(int decimal, int size, char hex[]){
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



// Function to convert an integer to a binary string
void i2b(unsigned int num, char* output, int bit_length) {
	for (int i = bit_length - 1; i >= 0; i--) {
		output[bit_length - i - 1] = (num & (1 << i)) ? '1' : '0';
	}
	output[bit_length] = '\0';  // Null-terminate the string
}


/* update_gflags: Update Global Flags */
void update_gflags(GFLAGS *gflags, int argc, char *argv[]){
	gflags->stepping = 0;
	gflags->is_pause = 0;
	gflags->frequency = 500000;
	gflags->pload = PROGRAM_LOAD;
	gflags->is_sleep = 0;
	gflags->console_en = 0;
	memset(gflags->program, '\0', MALL);
	memset(gflags->load, '\0', MALL);
	// int ps, fs, ls = 0; // program save
	int ps = 0;  // program save
	int fs = 0;  // File save
	int ls = 0;  // load save

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
void end_sig_func(){
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
	system("clear");  // clear screen when start the program
}


/* get use char with no interruption */
// int getc_keep(void) {
//     char ch;
// 
//     // Read a character from stdin
//     if (read(STDIN_FILENO, &ch, 1) == 1) {
//         return ch;
//     }
// 
//     return -1;
// }


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





int get_key(void) {
    char ch;

    // Read a character from stdin
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        // Convert character to a number and adjust it
        ch = ch - '1';

        // Check if the adjusted value is within the desired range
        if (ch >= 0 && ch <= 7) {
            return 0b00000000 | (1 << ch);  // Return bitmask for keys 1-8
        } else {
            return -1;  // Return -1 if the input is out of range
        }
    }

    return -1;  // Return -1 if read fails or no input is available
}

int is_input_on(int inpt, int idx){
	idx = idx % 8;
	int binary = (inpt & ~get_w_reg());
	return edfb(binary, idx + 1, idx + 1) == 1;
}


int is_bit_input(int idx){
	idx = idx % 8;
	return edfb(get_w_reg(), idx + 1, idx + 1) == 0;
}


void binary_led(int num, uint8_t keynum, char binary[], int len){
	memset(binary, 0, len);

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
	}
}

// #include <signal.h>
// #include <stdlib.h>
// #include <termios.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdarg.h>
// #include <fcntl.h>
// #include <sys/ioctl.h>
// #include <unistd.h>
// #include <wchar.h>
// #include "components/mem.h"
// #include "rules.h"
// #include "types.h"
// #include "term.h"
// #include "display.h"
// 
// 
// /* Decimal TO Binary: convert given decimal to binary (char *) */
// void dtob(int decimal_num, int len, char binary_str[]) {
// 	len = len + 2;
// 
// 	// Fill with '0'
// 	memset(binary_str, '0', len);
// 	binary_str[len] = '\0';
// 
// 	// Add '0b' prefix
// 	binary_str[0] = '0';
// 	binary_str[1] = 'b';
// 
// 	// Convert decimal to binary and fill from the end
// 	int index = len - 1;
// 	while (decimal_num > 0 && index > 1) {
// 		binary_str[index] = (decimal_num % 2) + '0';
// 		decimal_num /= 2;
// 		index--;
// 	}
// }
// 
// 
// /* dtoh: (Decimal TO Hex) converts given decimal into hex string with size of 'siz' */
// char *dtoh(int decimal, int size, char hex[]){
// 	if (hex == NULL) {
// 		return NULL;
// 	}
// 
// 	hex[0] = '0';
// 	hex[1] = 'x';
// 	hex[size + 2] = '\0'; // Add null terminator
// 
// 	// Handle padding
// 	for (int i = size + 1; i >= 2; i--) {
// 		if (decimal == 0 && i > 2) {
// 			hex[i] = '0';
// 		} else {
// 			int digit = decimal % 16;
// 			if (digit < 10) {
// 				hex[i] = '0' + digit;
// 			} else {
// 				hex[i] = 'A' + digit - 10;
// 			}
// 			decimal /= 16;
// 		}
// 	}
// 	return hex;
// }
// 
// 
// 
// /* Function to convert an integer to a binary string */
// void i2b(unsigned int num, char* output, int bit_length) {
// 	for (int i = bit_length - 1; i >= 0; i--) {
// 		output[bit_length - i - 1] = (num & (1 << i)) ? '1' : '0';
// 	}
// 	output[bit_length] = '\0';  // Null-terminate the string
// }
// 
// 
// /* update_gflags: Update Global Flags */
// void update_gflags(GFLAGS *gflags, int argc, char *argv[]){
// 	gflags->stepping = 0;
// 	gflags->is_pause = 0;
// 	gflags->frequency = 500000;
// 	gflags->pload = PROGRAM_LOAD;
// 	gflags->is_sleep = 0;
// 	gflags->console_en = 0;
// 
// 	memset(gflags->program, '\0', MALL);
// 	memset(gflags->load, '\0', MALL);
// 	// int ps, fs, ls = 0; // program save
// 	int ps = 0;  // program save
// 	int fs = 0;  // File save
// 	int ls = 0;  // load save
// 
// 	int i;
// 	for(i = 0; i < argc; ++i){
// 
// 		if(ps == 1){
// 			strcpy(gflags->program, argv[i]);
// 			ps = 0;
// 			continue;
// 		}
// 		
// 		if(ls == 1){
// 			strcpy(gflags->load, argv[i]);
// 			ls = 0;
// 			gflags->pload = STATE_LOAD;
// 			continue;
// 		}
// 
// 		if(fs == 1){
// 			gflags->frequency = atoi(argv[i]);
// 
// 			if(gflags->frequency == 0){
// 				gflags->frequency = 500000;
// 			} else if (gflags->frequency > 1000000){
// 				gflags->frequency = 1;
// 			} else{
// 				gflags->frequency = 1000000 / gflags->frequency;
// 			}
// 
// 			fs = 0;
// 			continue;
// 		}
// 
// 		for(int j = 0; j < (int)strlen(argv[i]); ++j){
// 			if(argv[i][0] == '-'){
// 				switch(argv[i][j]){
// 					case 's':
// 						gflags->stepping = 1;
// 						break;
// 					case 'c':
// 						gflags->console_en = 1;
// 						break;
// 					case 'p':
// 						ps = 1;
// 						break;
// 					case 'f':
// 						fs = 1;
// 						break;
// 					case 'l':
// 						ls = 1;
// 						break;
// 					default:
// 						break;
// 				}
// 			}
// 		}
// 
// 	}
// 
// 	if(ps == 1){
// 		lprt(1, "After [afaf00]'-p'[ffffff] you should put the path to the supported [f44336].bin[ffffff] file!");
// 	}
// 
// 	if(fs == 1){
// 		lprt(1, "After [afaf00]'-f'[ffffff] you should put clock frequency!");
// 	}
// 
// 	if(ls == 1){
// 		lprt(1, "After [afaf00]'-l'[ffffff] you should put save state file!");
// 	}
// 
// 	if(strcmp(gflags->program, "") == 0 && strcmp(gflags->load, "") == 0){
// 		lprt(1, "By using [afaf00]'-p <path>'[ffffff] specify the path to the program file!");
// 	}
// }
// 
// 
// 
// /************************** Signals **************************/
// struct sigaction old_action;
// 
// // Action for end of the program signal <C-c>
// void end_sig_func(){
// 	normal_terminal();
// 	sigaction(SIGINT, &old_action, NULL);
// 	kill(0, SIGINT);
// }
// 
// // capture end of program signal <C-c>
// void init_end_sig(){
// 	struct sigaction action;
// 	memset(&action, 0, sizeof(action));
// 	action.sa_handler = &end_sig_func;
// 	sigaction(SIGINT, &action, &old_action);
// 	system("clear");  // clear screen when start the program
// }
// 
// 
// /* get a char from user with no interruption */
// int getc_keep(void){
// 	fd_set readfds;
// 	struct timeval tv;
// 	char ch;
// 
// 	// Set stdin to non-blocking mode
// 	int flags = fcntl(STDIN_FILENO, F_GETFL);
// 	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
// 
// 	FD_ZERO(&readfds);
// 	FD_SET(STDIN_FILENO, &readfds);
// 	tv.tv_sec = 0;
// 	tv.tv_usec = 0; // Check immediately
// 
// 	int ready = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
// 	if (ready == 1){
// 		read(STDIN_FILENO, &ch, 1);
// 		return ch;
// 	}
// 
// 	return -1;
// }
// 
// 
// /* save_cpu_state: save current state of CPU to a file */
// void save_cpu_state(GFLAGS gf, int pc){
// 	FILE *fp;
// 	fp = fopen("./cpu_state.txt", "w+");
// 	if(fp == NULL)
// 		return;
// 	fprintf(fp, "%s\n", gf.program);
// 	fprintf(fp, "%d\n", gf.frequency);
// 	fprintf(fp, "%d\n", pc);
// 	int i;
// 	for(i = 0; i < REGSIZ; ++i){
// 		fprintf(fp, "%d\n", REGISTERS[i]);
// 	}
// 	for(i = 0; i < RAMSIZ; ++i){
// 		fprintf(fp, "%d\n", RAM[i]);
// 	}
// 	fclose(fp);
// }
// 
// 
// /* load_cpu_state: Load state of CPU from a file and return the PC */
// int load_cpu_state(GFLAGS *gf) {
// 	FILE *fp;
// 
// 	int pc = 0;
// 
// 	fp = fopen(gf->load, "r");
// 	if(fp == NULL){ return -1; }
// 
// 	if (fscanf(fp, "%s\n%d\n%d\n", gf->program, &gf->frequency, &pc) != 3) {
// 		fprintf(stderr, "Error reading file: Invalid format\n");
// 		fclose(fp);
// 		return -1;
// 	}
// 
// 	for (int i = 0; i < REGSIZ; ++i) {
// 		if (fscanf(fp, "%d\n", (int *)&REGISTERS[i]) != 1){
// 			fclose(fp);
// 			return -1;
// 		}
// 	}
// 
// 	for (int i = 0; i < RAMSIZ; ++i) {
// 		if (fscanf(fp, "%d\n", (int *)&RAM[i]) != 1) {
// 			fclose(fp);
// 			return -1;
// 		}
// 	}
// 
// 	fclose(fp);
// 	return pc;
// }
// 
// 
// /* Extract Decimal From Binary (start and end are included)*/
// int edfb(int decimal, int start, int end) {
// 	start--;
// 	end--;
// 
// 	// Create a mask to extract the desired bits
// 	int mask = (1 << (end - start + 1)) - 1;
// 
// 	// Shift the mask to the correct position
// 	mask <<= start;
// 
// 	// Extract the desired bits
// 	int ebits = (decimal & mask) >> start;
// 	return ebits;
// }
// 
// 
// /* Get keyboard key with no interruption & no echo */
// int get_key(void){
// 	fd_set readfds;
// 	struct timeval tv;
// 	char ch;
// 
// 	// Set stdin to non-blocking mode
// 	int flags = fcntl(STDIN_FILENO, F_GETFL);
// 	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
// 
// 	FD_ZERO(&readfds);
// 	FD_SET(STDIN_FILENO, &readfds);
// 	tv.tv_sec = 0;
// 	tv.tv_usec = 0; // Check immediately
// 
// 	int ready = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
// 	if (ready == 1) {
// 		read(STDIN_FILENO, &ch, 1);
// 		// return ch;  // if you want to return char
// 
// 		ch = ch - '1';
// 		if(ch >= 0 && ch <= 7){
// 			return 0x00 | 1 << (ch);  // 0b00000000 | 1 << (c)
// 		} else {
// 			return -1;
// 		}
// 	}
// 	return -1;
// }
// 
// 
// /* is_input_on: Check that if bit is 1 (GPIO)*/
// int is_input_on(int inpt, int idx){
// 	idx = idx % 8;
// 	int binary = (inpt & ~get_w_reg());
// 	return edfb(binary, idx + 1, idx + 1) == 1;
// }
// 
// /* is_bit_input: check that given bit is for input (GPIO)*/
// int is_bit_input(int idx){
// 	idx = idx % 8;
// 	return edfb(get_w_reg(), idx + 1, idx + 1) == 0;
// }
// 
// 
// /* Binary LED (input: red, output: green) for GPIO (0x06) */
// void binary_led(int num, uint8_t keynum, char binary[], int len){
// 	memset(binary, 0, len);
// 
// 	for (int i = 7; i >= 0; i--) {
// 		if(INPUT_EN && is_bit_input(i)){
// 			if(is_input_on(keynum, i)){
// 				strcat(binary, "[FF0000]❚");
// 			} else {
// 				strcat(binary, "[909090]❚");
// 			}
// 		} else {
// 			if (num & (1 << i)) {
// 				strcat(binary, "[00FF00]❚");
// 			} else {
// 				strcat(binary, "[909090]❚");
// 			}
// 		}
// 	}
// }
// 
