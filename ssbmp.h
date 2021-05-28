#ifndef __SSBMP_H__
#define __SSBMP_H__

#include <pspkernel.h>
#include <pspsdk.h>
#include <pspsysclib.h>
#include <pspdisplay.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <malloc.h>

#include "mymalloc.h"
#include "thread.h"
#include "ss.h"
#include "sscommon.h"
#include "nidresolve.h"

#define BMP_ID		"BM"
#define BMP_ID_SIZE	2

#define BIT_COUNT 24

typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef long            LONG;

typedef struct tagBITMAPFILEHEADER {
        DWORD   bfSize;
        DWORD   bfReserved;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
} BITMAPINFOHEADER;

#endif //__SSBMP_H__
