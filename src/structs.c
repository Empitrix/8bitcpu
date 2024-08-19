#include "rules.h"


typedef struct ROM {
	int mcode[ROMSIZ];  // Machine Code
} ROM;


typedef struct RAM {
	int registers[REGSIZ];
} RAM;


typedef struct FETCH {
	int pc;     // Program Counter
	int data;   // binary data
} FETCH;

typedef struct EXEC {
	char *opcode;     // Operation Code
} EXEC;

typedef struct GFLAGS {
	int stepping;
} GFLAGS;




typedef enum OPCODES {
	NOP_OP  = 0,  // No OPeration
	BCF_OP  = 4,  // Bit Clear File
	BSF_OP  = 5,  // Bit Scan Forward
	GOTO_OP = 10, // GOTO
} OPCODES;

typedef struct DECODE {
	OPCODES opcode;
	int operand;
} DECODE;


typedef struct TERSIZ {
	int x;
	int y;
} TERSIZ;


