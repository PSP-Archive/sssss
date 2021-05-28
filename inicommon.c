/*
	PNGÇ∆BMPópÇÃINIì«Ç›çûÇ›ä÷êî
*/

#include <string.h>
#include <libini.h>
#include <pspctrl.h>
#include "common.h"

void loadIni(const char* file, u32 *buttons)
{
	char buf[BUF_SIZE];
	ini_pair list[1];
	ini_data data;
	memset(&data, 0, sizeof(ini_data));
	memset(list, 0, sizeof(list));

	data.pair	= list;
	data.size	= sizeof(list);
	data.buf	= buf;
	data.bufLen	= BUF_SIZE;
	
	*buttons = PSP_CTRL_NOTE;
	iniAddKey(&data, INI_KEY_BUTTONS, INI_TYPE_HEX, buttons, PSP_CTRL_NOTE);
	iniLoad(file, &data, NULL, NULL);
}
