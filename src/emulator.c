#include "components/rom.h"
#include "components/exec.h"
#include "components/reg.h"
#include "rules.h"
#include "structs.h"
#include "utils.h"
#include "display.h"
#include <stdio.h>



/* emulate_cpu: display CPU's data as a TUI */
void emulate_cpu(ROM rom, DECODE dcd, EXEC exec, REG reg, RAM ram, GFLAGS flags){
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
		" [55B6C2]PC[]: [ed400e]%-4d[] [55B6C2]GPIO[]: [ed400e]%s[]  %s[{}]",
		get_pc(),
		dtoh(reg.registers[6], 2),
		dtob_led(reg.registers[6], 8));


	// ROM
	int max_h = (ts.y) - 6;
	int linen = 0;

	if(get_pc() > max_h / 2){
		linen = (get_pc() % ROMSIZ) - (max_h / 2);
		if(linen + (max_h / 2) + (max_h / 2) + 2 > ROMSIZ)
			linen = ROMSIZ - max_h;
	}
	// ROM list view
	for(int i = 0; i < max_h; ++i){
		if((linen + i)  == get_pc())
			dprt(4, 4 + i, "%s%-4s [{}][u]%s", i == 0 ? "[F44336]" : "[fcd200]", dtoh(i, 4), exec_info(rom.mcode[i]));
		else
			dprt(4, 4 + i, "%s%-4s [{}]%s", i == 0 ? "[D32F2F]" : "[808080]", dtoh(i, 4), exec_info(rom.mcode[i]));
	}


	// REG
	for(int i = 0; i < REGSIZ; ++i)
		dprt(hx + 5, 4 + i,
			"%-16s [E06B74]%s [98C379]%s",
			get_reg_name(i),
			dtob(reg.registers[i], 8),
			dtoh(reg.registers[i], 2));


	int ram_ps = ts.y - 18;  // ram pannel size
	// RAM
	for(int i = 0; i < (ram_ps >= 16 ? 16 : ram_ps); ++i)
		dprt(hx + 5, 16 + i,
			"[ABB2BF]%-4s [E06B74]%s [98C379]%s",
			dtoh(i + 16, 2),
			dtob(ram.ram[i], 8),
			dtoh(ram.ram[i], 2));


	// Info
	dprt(hx + 33, 4, "[2196F3]Mode[FFFFFF]: [8bc34a]%s", flags.stepping ? "Stepping" : "Auto");
	if(flags.stepping)
		dprt(hx + 33, 5, "[2196F3]Frequency[FFFFFF]: [FFDFAF]Keyboard Key");
	else
		dprt(hx + 33, 5, "[2196F3]Frequency[FFFFFF]: [FFDFAF]%d", flags.frequency);
	dprt(hx + 33, 6, "[2196F3]Program[FFFFFF]: [98C379]\"%s\"", flags.program);


	fflush(NULL);  // Flush the output (ALL)
}
