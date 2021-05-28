/*
	画像処理用共通関数
*/

#include "sscommon.h"


void *getLineBuf(){
	return malloc(sizeof(BYTE) * PIXEL_SIZE * SCREEN_WIDTH);
}

void getDisplayStat(void **topaddr, int *pwidth, int *pheight, int *bufferwidth, int *pixelformat, int *buffersize)
{
	nidResolve();

	int pmode;

	//ディスプレイ情報取得
	sceDisplayWaitVblankStart();
	sceDisplayGetMode(&pmode, pwidth, pheight);
	sceDisplayGetFrameBuf(topaddr, bufferwidth, pixelformat, PSP_DISPLAY_SETBUF_NEXTFRAME);

	*buffersize = (*bufferwidth) * (*pwidth) * ((*pixelformat) == PSP_DISPLAY_PIXEL_FORMAT_8888? sizeof(u32): sizeof(u16));
}

void vramCopy(void *dst, const void *src, int size)
{
	int i;

	BYTE *dstVram = (BYTE *)dst;
	BYTE *srcVram = (BYTE *)src;

	// vramはmemcpyが遅いという話を聞いた
	for(i = 0; i < size ; i++)
		*dstVram++ = *srcVram++;
}

void setPixelColor(BYTE *pixel, int pixelformat, u32 *vram32, u32 offset)
{
	u16 *vram16 = (u16 *)vram32;
	u32 color = 0;
	BYTE r = 0, g = 0, b = 0;

	switch (pixelformat) {
		case PSP_DISPLAY_PIXEL_FORMAT_565:
			color = vram16[offset];
			r = (color & 0x1f) << 3; 
			g = ((color >> 5) & 0x3f) << 2 ;
			b = ((color >> 11) & 0x1f) << 3 ;
			break;
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
			color = vram16[offset];
			r = (color & 0x1f) << 3; 
			g = ((color >> 5) & 0x1f) << 3 ;
			b = ((color >> 10) & 0x1f) << 3 ;
			break;
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
			color = vram16[offset];
			r = (color & 0xf) << 4; 
			g = ((color >> 4) & 0xf) << 4 ;
			b = ((color >> 8) & 0xf) << 4 ;
			break;
		case PSP_DISPLAY_PIXEL_FORMAT_8888:
			color = vram32[offset];
			r = color & 0xff; 
			g = (color >> 8) & 0xff;
			b = (color >> 16) & 0xff;
			break;
	}

	pixel[0] = r;
	pixel[1] = g;
	pixel[2] = b;
}
