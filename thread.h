#ifndef __THREAD_H__
#define __THREAD_H__

#include <pspkernel.h>
#include <pspthreadman.h>
#include <stdbool.h>
#include <string.h>

enum
{
	LIBM_TCMD_SUSPEND	,	//���X���b�h���~
	LIBM_TCMD_RESUME	,	//���X���b�h���ĊJ
	LIBM_TCMD_DUMP		,	//���X���b�h���~�E�ĊJ������ׂ̃Z�[�t���X�g�����
	LIBM_TCMD_GET		,	//���X���b�h�̒�~�E�ĊJ��Ԃ��擾
};

#define MAX_NUMBER_OF_THREADS 		64

#define	PSP_SYSEVENT_SUSPEND_QUERY	0x100

int libmExecTCmd( int cmd );

#endif //__THREAD_H__
