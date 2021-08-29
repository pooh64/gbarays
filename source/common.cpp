extern "C" {
#include <gba_console.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
};

#include <common.h>

void error(char const *msg, char const *file, int line)
{
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleInit(0, 4, 0, 0, 0, 15);
	BG_COLORS[0] = 0;
	BG_COLORS[241] = RGB5(31, 31, 31);
	SetMode(MODE_0 | BG0_ON);
	iprintf("%s:%d\n\n%s", file, line, msg);
	while (1) {
		VBlankIntrWait();
	}
}