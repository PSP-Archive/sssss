/*
	���C���֐�
*/

#include "main.h"

PSP_MODULE_INFO(MODULE_NAME, PSP_MODULE_KERNEL, 1, 1);


int threadMain(SceSize args, void *argp)
{
	// �e���W���[���̓ǂݍ��݂�҂���NID����
	while( 1 )
	{
		if(
			sceKernelFindModuleByName("sceSystemMemoryManager") && 
			sceKernelFindModuleByName("sceRTC_Service") && 
			sceKernelFindModuleByName("sceDisplay_Service") && 
			sceKernelFindModuleByName("sceController_Service")
		){
			break;
		}

		sceKernelDelayThread(1000 * 1000);
	}
	nidResolve();

	// �X���b�h���X�g�쐬
	libmExecTCmd(LIBM_TCMD_DUMP);

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	
	pspTime rtime;
	SceCtrlData pad;
	u32 buttons, padLast = 0;

	char file[BUF_SIZE];
	char filename[BUF_SIZE];
	char *temp = NULL;

	memset(file,0,sizeof(file));
	memset(filename,0,sizeof(filename));
	strcpy(file, argp);
	temp = strrchr(file, '/');
	if( temp != NULL ) *temp = '\0';
	strcat(file, "/" MODULE_NAME ".ini");

	// �ݒ�t�@�C���ǂݍ���
	loadIni(file, &buttons);

	// �f�B���N�g���@��̂͋N��������
	// �N����ɃX���[�v�����č폜�Ƃ��C�ɂ��Ȃ�
	temp = strchr(file, '/');
	if( temp != NULL ) *temp = '\0';
	strcat(file, "/PICTURE");
	sceIoMkdir(file, 0777);
	strcat(file, "/capture");
	sceIoMkdir(file, 0777);

	// ���݂̓���N���b�N�ێ�
	// CFW���ł������Ă�ꍇ�͈Ӗ��Ȃ��񂾂�ˁc
	int clockCPU = scePowerGetCpuClockFrequency();
	int clockBUS = scePowerGetBusClockFrequency();

	while( 1 )
	{
	   	sceCtrlPeekBufferPositive(&pad, 1);
		
		if((pad.Buttons != padLast) && ((pad.Buttons & buttons) == buttons)){

			// �T�X�y���h�p�̃������̈����ƃo�b�t�@�Ɏg���̂œd���X�C�b�`�����b�N
			// �t�@�C���������ݒ��ɓd���؂���̂�������
			scePowerLock(0);

			// ��r�I�d�������Ȃ̂ŃN���b�N����
			scePowerSetClockFrequency(333,333,166);

			// ���X���b�h���T�X�y���h
			while(!libmExecTCmd( LIBM_TCMD_SUSPEND ));

			sceRtcGetCurrentClockLocalTime(&rtime);
			sprintf(
				filename, "%s/%04d%02d%02d%02d%02d%02d." MODULE_TYPE,
				file, rtime.year, rtime.month, rtime.day, rtime.hour, rtime.minutes, rtime.seconds
			);

			screenshot(filename);

			// ���X���b�h�����W���[��
			// screenshot���ŃG���[�����������p
			if( libmExecTCmd(LIBM_TCMD_GET) == LIBM_TCMD_SUSPEND ){
				while(!libmExecTCmd( LIBM_TCMD_RESUME ));
			}

			// ������������
			scePowerSetClockFrequency(clockCPU,clockCPU,clockBUS);
			scePowerUnlock(0);
		}
		padLast = pad.Buttons;

		sceKernelDelayThread( 10000 );
	}
	
	return sceKernelExitDeleteThread( 0 );
}


int module_start(SceSize args, void *argp)
{
	// �X�^�b�N�T�C�Y�Ă��Ƃ�
    SceUID main_thid = sceKernelCreateThread(MODULE_NAME "_Main", threadMain, 16,0x1500, 0, NULL);
	
    if(main_thid >= 0) sceKernelStartThread(main_thid, args, argp);
	
    return 0;
}

int module_stop(SceSize args, void *argp)
{
    return 0;
}
