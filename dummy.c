/*
	�摜���C�u��������Ă΂��֐����_�~�[�ō쐬�B
	���쎞�ɂ͂���炪�K�v�ȏ������Ă΂Ȃ��悤�ɂ���̂œK�������B
*/

#include <stdio.h>
#include <string.h>
#include "common.h"

struct _reent *_impure_ptr __ATTRIBUTE_IMPURE_PTR__;

// ���ꂾ�����Ƃ܂Ƃ��B�g��Ȃ����ǁB
int fprintf(FILE *pOut, const char *zFormat, ...)
{
	va_list ap;
	int ret;

	char buf[BUF_SIZE];

	va_start( ap, zFormat );
	ret = vsnprintf( buf, BUF_SIZE, zFormat, ap );
	va_end( ap );

	return (ret >= 0)? fwrite(buf, strlen(buf), 1, pOut): 0;
}

int sscanf(const char *buf, const char *format, ...){
	return 0;
}

char *getenv(const char *env){
	return (char *)NULL;
}

struct tm *gmtime(const time_t *timer){
	return NULL;
}

void abort(void){
	while (1) sceKernelExitThread(0);
}


void exit(int status){
	while (1) sceKernelExitThread(status);
}

