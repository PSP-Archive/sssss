/*
	画像ライブラリから呼ばれる関数をダミーで作成。
	動作時にはこれらが必要な処理を呼ばないようにするので適当実装。
*/

#include <stdio.h>
#include <string.h>
#include "common.h"

struct _reent *_impure_ptr __ATTRIBUTE_IMPURE_PTR__;

// これだけ割とまとも。使わないけど。
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

