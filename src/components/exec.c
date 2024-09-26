#include "../types.h"
#include "./decode.h"
#include "./mem.h"
#include "rom.h"
#include <stdio.h>


/* return EXEC that contains execute information for given instruction */
/* Soft in soft_execute means it's not memory (register, ram ...) related */
EXEC soft_execute(DECODE dcd){
	EXEC exec;

	exec.sleep = 0;
	exec.upc = get_pc();  // Default PC

	switch(dcd.opcode) {
		case GOTO_OP:
			exec.upc = dcd.operand;
			break;

		case SLEEP_OP:
			exec.sleep = 1;
			break;

		case CALL_OP:
			push_stack(get_pc() + 1);   // (PC + 1) to Stack-1
			exec.upc = dcd.addr;
			break;

		case RETLW_OP:
			exec.upc = pop_stack();     // Stack-1 to PC
			set_w_reg(dcd.bits);        // Load Register W
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

	set_sfr(reg, PCL_REGISTER, get_pc());

	// FSR, INDF (set INDF by content of FSR)
	int fsr = get_sfr(reg, FSR_REGISTER);
	set_sfr(reg, INDF_REGISTER, fsr);

	set_sfr(reg, TMR0_REGISTER, get_cpu_coutner());
	


	// Update Carry bit in STATUS
	if(get_carry()){
		set_sfr_bit(reg, STATUS_REGISTER, 0);
	} else {
		clear_sfr_bit(reg, STATUS_REGISTER, 0);
	}



	switch(dcd.opcode) {
		// BSF
		case BSF_OP:
			m = get_mem(reg, ram, dcd.addr);
			set_mem(reg, ram, m, m.value | (1 << dcd.bits));
			break;

		// BCF
		case BCF_OP:
			m = get_mem(reg, ram, dcd.addr);
			set_mem(reg, ram, m, m.value & ~(1 << dcd.bits));
			break;

		// MOVLW
		case MOVLW_OP:
			set_w_reg(dcd.bits);
			break;

		// MOVWF
		case MOVWF_OP:
			m = get_mem(reg, ram, dcd.addr);
			set_mem(reg, ram, m, get_w_reg());
			break;

		// CLRF
		case CLRF_OP:
			m = get_mem(reg, ram, dcd.addr);
			set_mem(reg, ram, m, 0);
			break;
		
		// CLRW
		case CLRW_OP:
			set_w_reg(0);
			break;

		// DECF
		case DECF_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(m.value != 0){
				m.value--;
				clear_sfr_bit(reg, STATUS_REGISTER, 2);
			} else {
				set_sfr_bit(reg, STATUS_REGISTER, 2);
			}
			update_by_dist(reg, ram, m, dcd);
			break;

		// DECFSZ
		case DECFSZ_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(m.value != 0){
				m.value--;
				clear_sfr_bit(reg, STATUS_REGISTER, 2);
			} else {
				set_sfr_bit(reg, STATUS_REGISTER, 2);
			}
			update_by_dist(reg, ram, m, dcd);
			if(m.value == 0){ bypass = 1; }
			break;

		// INCF
		case INCF_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(m.value != 255){
				m.value++;
				clear_sfr_bit(reg, STATUS_REGISTER, 2);
			} else {
				m.value = 0;
				set_sfr_bit(reg, STATUS_REGISTER, 2);
			}
			update_by_dist(reg, ram, m, dcd);
			break;

		// INCFSZ
		case INCFSZ_OP:
			m = get_mem(reg, ram, dcd.addr);
			if(m.value != 255){
				m.value++;
				clear_sfr_bit(reg, STATUS_REGISTER, 2);
			} else {
				m.value = 0;
				set_sfr_bit(reg, STATUS_REGISTER, 2);
			}
			update_by_dist(reg, ram, m, dcd);
			if(m.value == 0){ bypass = 1; }
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
			if(m.value > 255){
				m.value = 0;
				clear_sfr_bit(reg, STATUS_REGISTER, 1);
				set_sfr_bit(reg, STATUS_REGISTER, 2);
			} else {
				set_sfr_bit(reg, STATUS_REGISTER, 1);
				clear_sfr_bit(reg, STATUS_REGISTER, 2);
			}
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
			if(m.value < 0){
				m.value = 0;
				clear_sfr_bit(reg, STATUS_REGISTER, 1);
				set_sfr_bit(reg, STATUS_REGISTER, 2);
			} else {
				set_sfr_bit(reg, STATUS_REGISTER, 1);
				clear_sfr_bit(reg, STATUS_REGISTER, 2);
			}

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
			set_sfr_bit(reg, STATUS_REGISTER, 3);
			set_sfr_bit(reg, STATUS_REGISTER, 4);
			break;

		// IORLW
		case IORLW_OP:
			set_w_reg(get_w_reg() | dcd.bits);
			break;

		// OPTION
		case OPTION_OP:
			set_sfr(reg, OPTION_REGISTER, get_w_reg());
			break;


		// CLRWDT
		case TRIS_OP:
			set_sfr(reg, TRISGPIO_REGISTER, dcd.addr);
			if(dcd.addr == 6){
				INPUT_EN = 1;
			} else {
				INPUT_EN = 0;
			}
			break;

		// XORLW
		case XORLW_OP:
			set_w_reg(get_w_reg() ^ dcd.bits);
			break;


		default:
			break;
	}

	increase_cc();  // increase cpu timer (counter)
	return bypass;
}

