#ifndef __THREAD_H__
#define __THREAD_H__

#include <pspkernel.h>
#include <pspthreadman.h>
#include <stdbool.h>
#include <string.h>

enum
{
	LIBM_TCMD_SUSPEND	,	//他スレッドを停止
	LIBM_TCMD_RESUME	,	//他スレッドを再開
	LIBM_TCMD_DUMP		,	//他スレッドを停止・再開させる為のセーフリストを作る
	LIBM_TCMD_GET		,	//他スレッドの停止・再開状態を取得
};

#define MAX_NUMBER_OF_THREADS 		64

#define	PSP_SYSEVENT_SUSPEND_QUERY	0x100

int libmExecTCmd( int cmd );

#endif //__THREAD_H__
