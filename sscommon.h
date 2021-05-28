#ifndef __SSCOMMON_H__
#define __SSCOMMON_H__

#include <pspkernel.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspsysclib.h>
#include <pspdisplay.h>
#include <pspsysevent.h>
#include <psprtc.h>
#include <psppower.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "common.h"
#include "mymalloc.h"
#include "thread.h"
#include "ss.h"
#include "nidresolve.h"

void *getLineBuf();
bool initScreenshot(FILE **fp, const char *file, BYTE **linebuf);
void abortScreenshot(FILE *fp, const char *file, BYTE *linebuf);
void destScreenshot(FILE *fp, BYTE *linebuf);

void getDisplayStat(void **topaddr, int *pwidth, int *pheight, int *bufferwidth, int *pixelformat, int *buffersize);
void vramCopy(void *dst, const void *src, int size);
void setPixelColor(BYTE *pixel, int pixelformat, u32 *vram32, u32 offset);

#endif //__SSCOMMON_H__
