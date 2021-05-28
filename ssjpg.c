/*
	JPEG処理本体
	JPEGだけ設定ファイルが違うのでここで一緒に定義
*/

#include <libini.h>
#include "ssjpg.h"

#define COMPRESS_DEFAULT 80
static int compress = COMPRESS_DEFAULT;

static jmp_buf jbuf;

// エラーは即throw
static void my_error_exit( j_common_ptr cinfo )
{
	longjmp( jbuf, 1 );
}

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

	vramCopy(vram32, vram32orig, size);
	if( libmExecTCmd(LIBM_TCMD_GET) == LIBM_TCMD_SUSPEND ){
		while(!libmExecTCmd( LIBM_TCMD_RESUME ));
	}

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	if (setjmp(jbuf) == 0){ // try

		cinfo.err = jpeg_std_error( &jerr );

		// エラーハンドラ設定
		jerr.error_exit = my_error_exit;

		jpeg_create_compress( &cinfo );
		jpeg_stdio_dest( &cinfo, fp );

		cinfo.image_width = SCREEN_WIDTH;
		cinfo.image_height = SCREEN_HEIGHT;
		cinfo.input_components = PIXEL_SIZE;
		cinfo.in_color_space = JCS_RGB;

		jpeg_set_defaults( &cinfo );
		jpeg_set_quality( &cinfo, compress, TRUE );
		jpeg_start_compress( &cinfo, TRUE );

		for(y = 0; y < ph; y++){
			for(i = 0, x = 0; x < pw; i += PIXEL_SIZE, x++){
				u32 offset = x + y * bufferwidth;
				setPixelColor(&linebuf[i], pixelformat, vram32, offset);
			}

			jpeg_write_scanlines( &cinfo, &linebuf, 1 );
		}

		jpeg_finish_compress( &cinfo );
		jpeg_destroy_compress( &cinfo );

		free(vram32);
		destScreenshot(fp, linebuf);

	}
	else { // catch
		jpeg_destroy_compress( &cinfo );

		free(vram32);
		abortScreenshot(fp, file, linebuf);

	}

}


void loadIni(const char* file, u32 *buttons)
{
	char buf[BUF_SIZE];
	ini_pair list[2];
	ini_data data;
	memset(&data, 0, sizeof(ini_data));
	memset(list, 0, sizeof(list));
	
	data.pair	= list;
	data.size	= sizeof(list);
	data.buf	= buf;
	data.bufLen	= BUF_SIZE;
	
	*buttons = PSP_CTRL_NOTE;
	compress = COMPRESS_DEFAULT;
	iniAddKey( &data, INI_KEY_BUTTONS, INI_TYPE_HEX, buttons, PSP_CTRL_NOTE );
	iniAddKey( &data, "compress", INI_TYPE_DEC, &compress, COMPRESS_DEFAULT );
	iniLoad(file,&data,NULL,NULL);

	if (0 > compress || compress > 100) compress = COMPRESS_DEFAULT;
}
