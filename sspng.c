/*
	PNG処理本体
*/

#include "sspng.h"

static jmp_buf jbuf;

// エラーは即throw
static void PNGAPI my_png_error(png_structp png_ptr, png_const_charp error_message)
{
	longjmp( jbuf, 1 );
}
// ワーニングは何もしない
// 標準ワーニング処理で何されるかわからないので作成
static void PNGAPI my_png_warning(png_structp png_ptr, png_const_charp warning_message){}


void screenshot(const char* file)
{
	int x, y, i;
	int pw, ph, bufferwidth, pixelformat, size;
	u32 *vram32, *vram32orig;
	u8 *linebuf;
	FILE *fp = NULL;

	if (!initScreenshot(&fp, file, &linebuf)) return;

	getDisplayStat((void *)&vram32orig, &pw, &ph, &bufferwidth, &pixelformat, &size);
	vram32 = (u32 *)malloc(size);
	if(!vram32){
		abortScreenshot(fp, file, linebuf);
		return;
	}

	png_structp png_ptr = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, NULL, my_png_error, my_png_warning
	); 
	if (!png_ptr){
		abortScreenshot(fp, file, linebuf);
		return;
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		abortScreenshot(fp, file, linebuf);
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return;
	}

	vramCopy(vram32, vram32orig, size);
	if( libmExecTCmd(LIBM_TCMD_GET) == LIBM_TCMD_SUSPEND ){
		while(!libmExecTCmd( LIBM_TCMD_RESUME ));
	}


	if (setjmp(jbuf) == 0){ // try

		png_init_io(png_ptr, fp);

		png_set_IHDR(
			png_ptr, info_ptr, SCREEN_WIDTH, SCREEN_HEIGHT,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
		);
		png_write_info(png_ptr, info_ptr);

		for(y = 0; y < ph; y++){
			for(i = 0, x = 0; x < pw; i += PIXEL_SIZE, x++){
				u32 offset = x + y * bufferwidth;
				setPixelColor(&linebuf[i], pixelformat, vram32, offset);
			}

			png_write_row(png_ptr, linebuf);
		}

		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);

		free(vram32);
		destScreenshot(fp, linebuf);

	}
	else { // catch
		png_destroy_write_struct(&png_ptr, &info_ptr);

		free(vram32);
		abortScreenshot(fp, file, linebuf);

	}

}
