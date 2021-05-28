/*
	BMP処理本体
	ライブラリを使わないので他2つとちょっと違う
*/

#include "ssbmp.h"

// エラー時用後始末
void abortBmp(SceUID fd, const char *file, void *linebuf, void *vram)
{
	sceIoClose(fd);
	sceIoRemove(file);
	free(linebuf);
	free(vram);
	mallocEnd();
}

void screenshot(const char* file)
{
	int x, y, i;
	int pw, ph, bufferwidth, pixelformat, size;
	u32 *vram32, *vram32orig;
	u8 *linebuf;

	SceUID fd = sceIoOpen(file, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_TRUNC, 0777);
	if(fd < 0) return;

	if (!mallocInit()){
		sceIoClose(fd);
		sceIoRemove(file);
		return;
	}

	linebuf = getLineBuf();
	if (!linebuf){
		abortBmp(fd, file, linebuf, NULL);
		return;
	}

	getDisplayStat((void *)&vram32orig, &pw, &ph, &bufferwidth, &pixelformat, &size);
	vram32 = (u32 *)malloc(size);
	if(!vram32){
		sceIoClose(fd);
		abortBmp(fd, file, linebuf, NULL);
		return;
	}

	vramCopy(vram32, vram32orig, size);
	if( libmExecTCmd(LIBM_TCMD_GET) == LIBM_TCMD_SUSPEND ){
		while(!libmExecTCmd( LIBM_TCMD_RESUME ));
	}


	// ヘッダ作成
	BITMAPFILEHEADER h1;
	BITMAPINFOHEADER h2;

	h1.bfSize	= pw * ph * PIXEL_SIZE + BMP_ID_SIZE + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	h1.bfReserved	= 0;
	h1.bfOffBits	= BMP_ID_SIZE + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	h2.biSize	= sizeof(BITMAPINFOHEADER);
	h2.biPlanes	= 1;
	h2.biBitCount	= BIT_COUNT;
	h2.biCompression 	= 0;
	h2.biWidth	= pw;
	h2.biHeight	= ph;
	h2.biSizeImage	= pw * ph * PIXEL_SIZE;
	h2.biXPelsPerMeter	= 0xEC4;
	h2.biYPelsPerMeter	= 0xEC4;
	h2.biClrUsed		= 0;
	h2.biClrImportant	= 0;


	// ヘッダ書き込み
	if (sceIoWrite(fd, BMP_ID, BMP_ID_SIZE) != BMP_ID_SIZE){
		abortBmp(fd, file, linebuf, vram32);
		return;
	}
	if (sceIoWrite(fd, &h1, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER)){
		abortBmp(fd, file, linebuf, vram32);
		return;
	}
	if (sceIoWrite(fd, &h2, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER)){
		abortBmp(fd, file, linebuf, vram32);
		return;
	}

	BYTE cTmp;
	int lineSize = pw * PIXEL_SIZE;
	// BMPは再下位ラインから処理
	for(y = (ph-1); y >= 0; y--){
		for(i = 0, x = 0; x < pw; i += PIXEL_SIZE, x++){
			u32 offset = x + y * bufferwidth;
			setPixelColor(&linebuf[i], pixelformat, vram32, offset);

			// RとB入れ替え
			cTmp = linebuf[i];
			linebuf[i] = linebuf[i+2];
			linebuf[i+2] = cTmp;
		}

		if (sceIoWrite(fd, linebuf, lineSize) != lineSize){
			abortBmp(fd, file, linebuf, vram32);
			return;
		}

	}

	sceIoClose(fd);
	free(linebuf);
	free(vram32);
	mallocEnd();
}
