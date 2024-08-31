#include <stdint.h>
#include "rules.h"


typedef struct ROM {
	int mcode[ROMSIZ];  // Machine Code
} ROM;


typedef struct RAM {
	uint8_t ram[RAMSIZ];
} RAM;


typedef struct REG {
	uint8_t registers[REGSIZ];
} REG;


typedef struct FETCH {
	int pc;     // Program Counter
	int data;   // Binary data
} FETCH;


typedef enum EXEC_TYPE {
	MONO_OP,
	MULTI_OP,
} exec_t;


typedef struct EXEC {
	char *opcode;  // Operation Code
	exec_t type;   // Number of operands
	int upc;       // Updated PC (Program Counter)
	int reg_n;     // Register Number
	int bit_n;     // Bit
} EXEC;


typedef enum CPU_LOAD {
	PROGRAM_LOAD,
	STATE_LOAD
} CPU_LOAD;


typedef struct GFLAGS {
	int stepping;
	int frequency;
	CPU_LOAD pload;
	char program[MALL];
	char load[MALL];
	int is_pause;
} GFLAGS;


/*
typedef enum OPCODES {
	NOP_OP  = 0,   // No OPeration
	BCF_OP  = 4,   // Bit Clear File
	BSF_OP  = 5,   // Bit Scan Forward
	GOTO_OP = 10,  // GOTO

	MOVLW_OP,
	MOVWF_OP,
	CLRF_OP,
	CLRW_OP,
	SLEEP_OP

} OPCODES;
*/

typedef enum OPCODES {
	NOP_OP,
	BCF_OP,
	BSF_OP,
	GOTO_OP,
	MOVLW_OP,
	MOVWF_OP,
	CLRF_OP,
	CLRW_OP,
	SLEEP_OP
} OPCODES;


typedef struct DECODE {
	OPCODES opcode;
	int operand;
} DECODE;


typedef struct TERSIZ {
	int x;
	int y;
} TERSIZ;


