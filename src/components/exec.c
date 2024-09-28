#include "../types.h"
#include "./decode.h"
#include "./mem.h"
#include "rom.h"


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


char* exec_info(int inst){
	char *info;

	DECODE dcd = decode_inst(inst);


	switch(dcd.type) {
		case FULL:
			info = setstr("%s0b%012b  %s%s %s%s", KB_P1, inst, K_HEX, dtoh(inst, 3), K_INFO, dcd.info);
			break;

		// KB_P1, edfb(inst, 7, 12), KB_P2, edfb(inst, 6, 6), KB_P3, edfb(inst, 1, 5),
		case SIX_ONE_FIVE:
			info = setstr("%s0b%06b%s%01b%s%05b  %s%s %s%s %s%s[FFFFFF], %s%s", KB_P1, edfb(inst, 7, 12), KB_P2, edfb(inst, 6, 6), KB_P3, edfb(inst, 1, 5), K_HEX, dtoh(inst, 3), K_INFO, dcd.info, K_OP1, dtoh(dcd.addr, 2), K_OP2, dtoh(dcd.bits, 2));
			break;

		// KB_P1, edfb(inst, 9, 12), KB_P2, edfb(inst, 6, 8), KB_P3, edfb(inst, 1, 5),
		case FOUR_THREE_FIVE:
			info = setstr("%s0b%04b%s%03b%s%05b  %s%s %s%s %s%s[FFFFFF], %s%s", KB_P1, edfb(inst, 9, 12), KB_P2, edfb(inst, 6, 8), KB_P3, edfb(inst, 1, 5), K_HEX, dtoh(inst, 3), K_INFO, dcd.info, K_OP1, dtoh(dcd.addr, 2), K_OP2, dtoh(dcd.bits, 2));
			break;

		// %s0b%04b%s%08b
		case FOUR_EIGHT:
			info = setstr("%s0b%04b%s%08b  %s%s %s%s %s%s", KB_P1, edfb(inst, 9, 12), KB_P2, edfb(inst, 1, 8), K_HEX, dtoh(inst, 3), K_INFO, dcd.info, K_OP1, dtoh(dcd.opcode == CALL_OP ? dcd.addr : dcd.bits, 2));
			break;

		case THREE_NINE:
			info = setstr("%s0b%03b%s%09b  %s%s %s%s %s%s", KB_P1, edfb(inst, 10, 12), KB_P2, edfb(inst, 1, 9), K_HEX, dtoh(inst, 3), K_INFO, dcd.info, K_OP1, dtoh(dcd.addr, 2));
			break;

		case SEVEN_FIVE:
			info = setstr("%s0b%07b%s%05b  %s%s %s%s %s%s", KB_P1, edfb(inst, 6, 12), KB_P2, edfb(inst, 1, 5), K_HEX, dtoh(inst, 3), K_INFO, dcd.info, K_OP1, dtoh(dcd.addr, 2));
			break;

		default:
			// sprintf(info, "[62AEEF]%s  [2979FF]%s [98C379]%s", "--------------", dtoh(inst, 3), "NOP");
			info = setstr("WOWO");
			break;
	}

	strcpy(info, update_color(info, 1));
	return info;
}




// put value into register 'W' if 'dcd.bits == 0' otherwise put if 'f' register
void update_by_dist(MEM_OUT mem, DECODE dcd){
	if(dcd.bits == 1){
		set_mem(mem, mem.value);
	} else {
		set_w_reg(mem.value);
	}
}


/* Execute & Update Memory etc... */
int execute(DECODE dcd){
	MEM_OUT m;
	int bypass = 0;
	int tmp = 0;

	// set_sfr(PCL_REGISTER, get_pc());

	// FSR, INDF (set INDF by content of FSR)
	int fsr = get_sfr(FSR_REGISTER);
	set_sfr(INDF_REGISTER, fsr);

	set_sfr(TMR0_REGISTER, get_cpu_coutner());
	


	// Update Carry bit in STATUS
	if(get_carry()){
		set_sfr_bit(STATUS_REGISTER, 0);
	} else {
		clear_sfr_bit(STATUS_REGISTER, 0);
	}



	switch(dcd.opcode) {
		// BSF
		case BSF_OP:
			m = get_mem(dcd.addr);
			set_mem(m, m.value | (1 << dcd.bits));
			break;

		// BCF
		case BCF_OP:
			m = get_mem(dcd.addr);
			set_mem(m, m.value & ~(1 << dcd.bits));
			break;

		// MOVLW
		case MOVLW_OP:
			set_w_reg(dcd.bits);
			break;

		// MOVWF
		case MOVWF_OP:
			m = get_mem(dcd.addr);
			set_mem(m, get_w_reg());
			break;

		// CLRF
		case CLRF_OP:
			m = get_mem(dcd.addr);
			set_mem(m, 0);
			break;
		
		// CLRW
		case CLRW_OP:
			set_w_reg(0);
			break;

		// DECF
		case DECF_OP:
			m = get_mem(dcd.addr);
			if(m.value != 0){
				m.value--;
				clear_sfr_bit(STATUS_REGISTER, 2);
			} else {
				set_sfr_bit(STATUS_REGISTER, 2);
			}
			update_by_dist(m, dcd);
			break;

		// DECFSZ
		case DECFSZ_OP:
			m = get_mem(dcd.addr);
			if(m.value != 0){
				m.value--;
				clear_sfr_bit(STATUS_REGISTER, 2);
			} else {
				set_sfr_bit(STATUS_REGISTER, 2);
			}
			update_by_dist(m, dcd);
			if(m.value == 0){ bypass = 1; }
			break;

		// INCF
		case INCF_OP:
			m = get_mem(dcd.addr);
			if(m.value != 255){
				m.value++;
				clear_sfr_bit(STATUS_REGISTER, 2);
			} else {
				m.value = 0;
				set_sfr_bit(STATUS_REGISTER, 2);
			}
			update_by_dist(m, dcd);
			break;

		// INCFSZ
		case INCFSZ_OP:
			m = get_mem(dcd.addr);
			if(m.value != 255){
				m.value++;
				clear_sfr_bit(STATUS_REGISTER, 2);
			} else {
				m.value = 0;
				set_sfr_bit(STATUS_REGISTER, 2);
			}
			update_by_dist(m, dcd);
			if(m.value == 0){ bypass = 1; }
			break;

		// BTFSS
		case BTFSS_OP:
			m = get_mem(dcd.addr);
			if(edfb(m.value, dcd.bits + 1, dcd.bits + 1) == 1){
				bypass = 1;
			}
			break;


		// BTFSC
		case BTFSC_OP:
			m = get_mem(dcd.addr);
			if(edfb(m.value, dcd.bits + 1, dcd.bits + 1) == 0){
				bypass = 1;
			}
			break;

		// ADDWF
		case ADDWF_OP:
			m = get_mem(dcd.addr);
			m.value = m.value + get_w_reg();
			if(m.value > 255){
				m.value = 0;
				clear_sfr_bit(STATUS_REGISTER, 1);
				set_sfr_bit(STATUS_REGISTER, 2);
			} else {
				set_sfr_bit(STATUS_REGISTER, 1);
				clear_sfr_bit(STATUS_REGISTER, 2);
			}
			update_by_dist(m, dcd);
			break;

		// ANDWF
		case ANDWF_OP:
			m = get_mem(dcd.addr);
			m.value &= get_w_reg();
			update_by_dist(m, dcd);
			break;

		// COMF
		case COMF_OP:
			m = get_mem(dcd.addr);
			m.value = ~m.value;
			update_by_dist(m, dcd);
			break;

		// IORWF
		case IORWF_OP:
			m = get_mem(dcd.addr);
			m.value |= get_w_reg();
			update_by_dist(m, dcd);
			break;

		// MOVF
		case MOVF_OP:
			m = get_mem(dcd.addr);
			update_by_dist(m, dcd);
			break;

		// RLF
		case RLF_OP:
			m = get_mem(dcd.addr);
			m.value = rotate_left_carry(m.value);
			update_by_dist(m, dcd);
			break;

		// RRF
		case RRF_OP:
			m = get_mem(dcd.addr);
			m.value = rotate_right_carry(m.value);
			update_by_dist(m, dcd);
			break;

		// SUBWF (W - f)
		case SUBWF_OP:
			m = get_mem(dcd.addr);
			m.value = get_w_reg() - m.value;
			if(m.value < 0){
				m.value = 0;
				clear_sfr_bit(STATUS_REGISTER, 1);
				set_sfr_bit(STATUS_REGISTER, 2);
			} else {
				set_sfr_bit(STATUS_REGISTER, 1);
				clear_sfr_bit(STATUS_REGISTER, 2);
			}

			update_by_dist(m, dcd);
			break;

		// SWAPF
		case SWAPF_OP:
			m = get_mem(dcd.addr);
			m.value = (m.value >> 4) | (m.value << 4);
			update_by_dist(m, dcd);
			break;

		// XORWF
		case XORWF_OP:
			m = get_mem(dcd.addr);
			m.value = m.value ^ get_w_reg();
			update_by_dist(m, dcd);
			break;

		// XORWF
		case ANDLW_OP:
			set_w_reg(get_w_reg() & dcd.bits);
			break;

		// CLRWDT
		case CLRWDT_OP:
			set_sfr_bit(STATUS_REGISTER, 3);
			set_sfr_bit(STATUS_REGISTER, 4);
			set_sfr(TMR0_REGISTER, 0);   // clear TIMER
			break;

		// IORLW
		case IORLW_OP:
			set_w_reg(get_w_reg() | dcd.bits);
			break;

		// OPTION
		case OPTION_OP:
			set_sfr(OPTION_REGISTER, get_w_reg());
			break;


		// CLRWDT
		case TRIS_OP:
			set_sfr(TRISGPIO_REGISTER, dcd.addr);
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

