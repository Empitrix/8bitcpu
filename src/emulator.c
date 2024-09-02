#include "components/rom.h"
#include "components/exec.h"
#include "components/reg.h"
#include "rules.h"
#include "structs.h"
#include "utils.h"
#include "display.h"
#include <stdio.h>
#include <string.h>



static int cw, ch = 0;
static int cflush = 0;
static char cbuff[50][MALL];

void make_size(int l_len){
	char tbuff[50][MALL];
	for(int i = 1; i < l_len; ++i){
		 strcpy(tbuff[i - 1], cbuff[i]);
	}

	memset(cbuff, 0, sizeof(cbuff));

	for(int i = 0; i < l_len - 1; ++i){
		strcpy(cbuff[i], tbuff[i]);
	}
}


void update_console(int x, int y, int inst, int max){

	int flush = edfb(inst, 8, 8);
	int cval = edfb(inst, 1, 7);

	if(flush && cflush == 0){
		if(cval == '\n'){
			ch++;
			cw = 0;
			if(ch == max){
				make_size(max);
				ch = max - 1;
			}
		} else{
			cbuff[ch][cw++] = cval;
		}
		cflush = 1;
	} else {
		cflush = 0;
	}

	// Display
	for(int i = 0; i < max; ++i){
		printf("%s%s", sgotoxy(x, y + i), cbuff[i]);
	}
}


/* emulate_cpu: display CPU's data as a TUI */
void emulate_cpu(ROM rom, DECODE dcd, REG reg, RAM ram, GFLAGS flags){
	if(flags.is_sleep == 1){
		return;
	}
	TERSIZ ts = term_size(); ts.y += 1;
	cls_term();

	// Main frame && header
	draw_box(1, 1, ts.x - 1, ts.y, " [98C379]8-BIT CPU[{}] ");

	int hx = (ts.x / 2);  // Half of x
	int hy = (ts.y / 2);  // Half of x

	// ROM Pannel
	draw_box(2, 3, hx, ts.y - 3, "ROM");
	// Registers Pannel
	draw_box(hx + 3, 3, 27, 13, "Registers");
	// Info Pannel
	draw_box(hx + 31, 3, ts.x - (hx + 32), 13, "INFO");
	// RAM Pannel
	draw_box(hx + 3, 15, 23, ts.y - 15, "RAM");
	// Console Pannel
	draw_box(hx + 27, 15, ts.x - (hx + 28), ts.y - 15, "Console");


	// Status Line
	dprt(2, 2,
		" [55B6C2]PC[]: [ed400e]%-4d[] [55B6C2]GPIO[]: [ed400e]%s[]  %s[{}]  [55B6C2]W-Reg[]:[] [ed400e]%s",
		get_pc(),
		dtoh(reg.registers[6], 2),
		dtob_led(reg.registers[6], 8),
		dtoh(get_w_reg(), 2)
	);


	// ROM
	int max_h = (ts.y) - 6;
	int linen = 0;

	if(get_pc() > (max_h / 2)){
		linen = (get_pc() % ROMSIZ) - (max_h / 2);
		if(linen + (max_h / 2) + (max_h / 2) + 2 > ROMSIZ){
			linen = ROMSIZ - max_h;
		}
	}

	// ROM list view
	for(int i = 0; i < max_h; ++i){
		if((linen + i) == get_pc()){
			dprt(4, 4 + i, "%s%-4s [{}][u]%s", (linen + i) == 0 ? "[F44336]" : "[fcd200]", dtoh(linen + i, 4), exec_info(rom.mcode[linen + i]));
		} else {
			dprt(4, 4 + i, "%s%-4s [{}]%s", (linen + i) == 0 ? "[D32F2F]" : "[808080]", dtoh(linen + i, 4), exec_info(rom.mcode[linen + i]));
		}
	}


	// REG
	for(int i = 0; i < REGSIZ; ++i){
		dprt(hx + 5, 4 + i,
			"%-16s [E06B74]%s [98C379]%s",
			get_reg_name(i),
			dtob(reg.registers[i], 8),
			dtoh(reg.registers[i], 2));
	}


	int ram_ps = ts.y - 18;  // ram pannel size
	// RAM
	for(int i = 0; i < (ram_ps >= 16 ? 16 : ram_ps); ++i){
		dprt(hx + 5, 16 + i,
			"[ABB2BF]%-4s [E06B74]%s [98C379]%s",
			dtoh(i + 16, 2),
			dtob(ram.ram[i], 8),
			dtoh(ram.ram[i], 2));
	}


	// Info
	dprt(hx + 33, 4, "[2196F3]Mode[FFFFFF]: [8bc34a]%s", flags.stepping ? "Stepping" : "Auto");
	if(flags.stepping){
		dprt(hx + 33, 5, "[2196F3]Frequency[FFFFFF]: [FFDFAF]Keyboard Key");
	} else {
		dprt(hx + 33, 5, "[2196F3]Frequency[FFFFFF]: [FFDFAF]%d", 1000000 / flags.frequency);
	}

	dprt(hx + 33, 7, "[2196F3]Program[FFFFFF]:");
	dprt(hx + 33, 8, "[98C379]\"%s\"", flags.program);


	// Update console (from register 6)
	if(flags.console_en){
		update_console(hx + 29, 16, reg.registers[6], ts.y - 18);
	} else {
		dprt(hx + 29, 16, "[B0B0B0]Use [D84315]'-c'[B0B0B0] to enable console");
	}

	fflush(NULL);  // Flush the output (ALL)
}
