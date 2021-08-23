#include <common.h>

void error(char const *msg, char const *file, int line)
{
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleDemoInit();
	//iprintf("\x1b[10;10Hassert: %s\n", str);
	iprintf("%s:%d: %s", file, line, msg);
	while (1) {
		VBlankIntrWait();
	}
}