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
	MONO_OPERAND,
	MULTI_OPERAND,
	FULL_OPERAND,
} exec_t;


typedef struct EXEC {
	int upc;       // Updated PC (Program Counter)
	int sleep;
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
	int is_sleep;
	int console_en;
} GFLAGS;


typedef enum OPCODES {
	NOP_OP,    // NO Operation
	BCF_OP,    // Bit Clear File
	BSF_OP,    // Bit Set Forward
	GOTO_OP,   // Goto
	MOVLW_OP,
	MOVWF_OP,
	CLRF_OP,
	CLRW_OP,

	DECF_OP,   // Decrement F
	DECFSZ_OP,

	INCF_OP,   // Increment F
	INCFSZ_OP,


	BTFSS_OP,   // Test bit F
	BTFSC_OP,

	SLEEP_OP   // Sleep
} OPCODES;


typedef struct TERSIZ {
	int x;
	int y;
} TERSIZ;


typedef enum mem_t {
	TO_RAM,
	TO_REG,
} mem_t;

/* Address to memory */
typedef struct MEM_OUT {
	mem_t type;
	int addr;
	int value;
	int valid;
} MEM_OUT;


/* Decode Instruction */
typedef struct DECODE {
	OPCODES opcode;  // OpCode
	int operand;     // everything except OpCode
	int bits;        // Part a Operand (bits)
	int addr;        // Part b Operand (addr)
	char *info;      // instruction info
	exec_t type;     // Instruction type (multi operand or mono)
} DECODE;

