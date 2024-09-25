#include "../structs.h"
#include "./decode.h"
#include "./mem.h"
#include "rom.h"
#include <stdio.h>


/* return EXEC that contains execute information for given instruction */
/* Soft in soft_execute means it's not memory (register, ram ...) related */
EXEC soft_execute(DECODE dcd){
	EXEC exec;

	exec.sleep = 0;
	// Default PC
	exec.upc = get_pc();

	switch(dcd.opcode) {
		case GOTO_OP:
			exec.upc = dcd.operand;
			break;

		case SLEEP_OP:
			exec.sleep = 1;
			break;

		case CALL_OP:
			save_call_pc(exec.upc + 1);
			exec.upc = dcd.addr;
			set_in_call(1);
			break;

		case RETLW_OP:
			if(get_in_call()){
				exec.upc = get_call_pc();
			}
			set_in_call(0);
			set_w_reg(dcd.bits);
			break;

	default:
		break;
	}

	return exec;
}


/* Return execution information for the given instruction */
char* exec_info(int inst){
	char *info = malloc(MALL * sizeof(char));

	DECODE dcd = decode_inst(inst);

	char *bits_1sec = dtob(inst, 12);
	char *bits_2sec = dtob2sec(inst, "[62AEEF]", "[E5C07A]");
	char *bits_3sec = dtob3sec(inst, "[62AEEF]", "[E5C07A]", "[C678DD]");

	switch(dcd.type) {
		case FULL_OPERAND:
			sprintf(info, "[62AEEF]%s  [2979FF]%s [98C379]%s", bits_1sec, dtoh(inst, 3), dcd.info);
			break;

		case MULTI_OPERAND:
			sprintf(info, "%s[FFFFFF]  [2979FF]%s [98C379]%s [ed400e]%s[FFFFFF], [E98C31]%s", bits_3sec, dtoh(inst, 3), dcd.info, dtoh(dcd.addr, 2), dtoh(dcd.bits, 2));
			break;

		case MONO_OPERAND:
			if(any_use_bit(dcd.opcode))
				sprintf(info, "%s[FFFFFF]  [2979FF]%s [98C379]%s [ed400e]%s", bits_2sec, dtoh(inst, 3), dcd.info, dtoh(dcd.operand, 2));
			else
				sprintf(info, "%s[FFFFFF]  [2979FF]%s [98C379]%s [ed400e]%s", bits_2sec, dtoh(inst, 3), dcd.info, dtoh(dcd.operand, 3));
			break;

		default:
			sprintf(info, "[62AEEF]%s  [2979FF]%s [98C379]%s", bits_1sec, dtoh(inst, 3), "NOP");
			break;
	}

	strcpy(info, update_color(info, 1));
	return info;
}


// put value into register 'W' if 'dcd.bits == 0' otherwise put if 'f' register
void update_by_dist(REG *reg, RAM *ram, MEM_OUT mem, DECODE dcd){
	if(dcd.bits == 1){
		set_mem(reg, ram, mem, mem.value);
	} else {
		set_w_reg(mem.value);
	}
}


/* Execute & Update Memory etc... */
int execute(DECODE dcd, REG *reg, RAM *ram){
	MEM_OUT m;
	int bypass = 0;
	int tmp = 0;

	switch(dcd.opcode) {

		// Set selected bit from the register to 1
		case BSF_OP:
			m = get_mem(reg, ram, dcd.addr);
			set_mem(reg, ram, m, m.value | (1 << dcd.bits));
			// reg->registers[dcd.addr] |= 1 << dcd.bits;
			break;

		// Set selected bit from the register to 0
		case BCF_OP:
			m = get_mem(reg, ram, dcd.addr);
			set_mem(reg, ram, m, m.value & ~(1 << dcd.bits));
			// reg->registers[dcd.addr] &= ~(1 << dcd.bits);
			break;

		case MOVLW_OP:
			set_w_reg(dcd.bits);
			break;

		case MOVWF_OP:
			m = get_mem(reg, ram, dcd.addr);
			set_mem(reg, ram, m, get_w_reg());
			// reg->registers[dcd.addr] = get_w_reg();
			break;

		case CLRF_OP:
			m = get_mem(reg, ram, dcd.addr);
			set_mem(reg, ram, m, 0);
			// reg->registers[dcd.addr] = 0;
			break;
		
		case CLRW_OP:
			set_w_reg(0);
			break;


		// DECF
		case DECF_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(m.value != 0){
				m.value--;
			}

			if(dcd.bits == 1){
				set_mem(reg, ram, m, m.value);
			} else {
				set_w_reg(m.value);
			}
			break;


		// DECFSZ
		case DECFSZ_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(m.value != 0){
				m.value--;
			}

			if(dcd.bits == 1){
				set_mem(reg, ram, m, m.value);
			} else {
				set_w_reg(m.value);
			}

			if(m.value == 0){
				bypass = 1;
			}

			break;



		// INCF
		case INCF_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(m.value != 255){
				m.value++;
			} else {
				m.value = 0;
			}

			if(dcd.bits == 1){
				set_mem(reg, ram, m, m.value);
			} else {
				set_w_reg(m.value);
			}
			break;



		// INCFSZ
		case INCFSZ_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(m.value != 255){
				m.value++;
			} else {
				m.value = 0;
			}

			if(dcd.bits == 1){
				set_mem(reg, ram, m, m.value);
			} else {
				set_w_reg(m.value);
			}

			if(m.value == 0){
				bypass = 1;
			}
			break;


		// BTFSS
		case BTFSS_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(edfb(m.value, dcd.bits + 1, dcd.bits + 1) == 1){
				bypass = 1;
			}
			break;


		// BTFSC
		case BTFSC_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(edfb(m.value, dcd.bits + 1, dcd.bits + 1) == 0){
				bypass = 1;
			}
			break;


		// ADDWF
		case ADDWF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value = m.value + get_w_reg();
			if(m.value > 255){ m.value = 0; }
			update_by_dist(reg, ram, m, dcd);
			break;

		// ANDWF
		case ANDWF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value &= get_w_reg();
			update_by_dist(reg, ram, m, dcd);
			break;

		// COMF
		case COMF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value = ~m.value;
			update_by_dist(reg, ram, m, dcd);
			break;

		// IORWF
		case IORWF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value |= get_w_reg();
			update_by_dist(reg, ram, m, dcd);
			break;

		// MOVF
		case MOVF_OP:
			m = get_mem(reg, ram, dcd.addr);
			update_by_dist(reg, ram, m, dcd);
			break;

		// RLF
		case RLF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value = rotate_left_carry(m.value);
			update_by_dist(reg, ram, m, dcd);
			break;

		// RRF
		case RRF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value = rotate_right_carry(m.value);
			update_by_dist(reg, ram, m, dcd);
			break;

		// SUBWF (W - f)
		case SUBWF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value = get_w_reg() - m.value;
			if(m.value < 0){ m.value = 0; }
			update_by_dist(reg, ram, m, dcd);
			break;

		// SWAPF
		case SWAPF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value = (m.value >> 4) | (m.value << 4);
			update_by_dist(reg, ram, m, dcd);
			break;

		// XORWF
		case XORWF_OP:
			m = get_mem(reg, ram, dcd.addr);
			m.value = m.value ^ get_w_reg();
			update_by_dist(reg, ram, m, dcd);
			break;

		// XORWF
		case ANDLW_OP:
			set_w_reg(get_w_reg() & dcd.bits);
			break;

		// CLRWDT
		case CLRWDT_OP:
			// TODO: The CLRWDT instruction resets the WDT. It also resets the prescaler, if the prescaler is assigned to the WDT and not Timer0. Status bits TO and PD are set.
			break;

		// IORLW
		case IORLW_OP:
			set_w_reg(get_w_reg() | dcd.bits);
			break;

		// OPTION
		case OPTION_OP:
			set_option_reg(get_w_reg());
			break;

		// XORLW
		case XORLW_OP:
			set_w_reg(get_w_reg() ^ dcd.bits);
			break;


		default:
			break;
	}

	return bypass;
}

