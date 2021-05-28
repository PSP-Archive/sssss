/*
	�摜�����p���[�e�B���e�B�֐�
	free�͌ĂԈӖ������񂾂��ǂ�
*/

#include "sscommon.h"

bool initScreenshot(FILE **fp, const char* file, BYTE **linebuf)
{

	if (!mallocInit()) return false;

	*linebuf = (BYTE *)getLineBuf();
	if (!(*linebuf)) return false;

	if (fp != NULL && file != NULL){
		*fp = fopen(file, "wb");
		if (!(*fp)) return false;
	}

	return true;
}

void abortScreenshot(FILE *fp, const char* file, BYTE *linebuf)
{
	if (fp != NULL) fclose(fp);
	if (file != NULL) sceIoRemove(file);
	free(linebuf);
	mallocEnd();
}

void destScreenshot(FILE *fp, BYTE *linebuf)
{
	if (fp != NULL) fclose(fp);
	free(linebuf);
	mallocEnd();
}

