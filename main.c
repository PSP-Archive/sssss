/*
	メイン関数
*/

#include "main.h"

PSP_MODULE_INFO(MODULE_NAME, PSP_MODULE_KERNEL, 1, 1);


int threadMain(SceSize args, void *argp)
{
	// 各モジュールの読み込みを待ってNID解決
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

	// スレッドリスト作成
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

	// 設定ファイル読み込み
	loadIni(file, &buttons);

	// ディレクトリ掘るのは起動時だけ
	// 起動後にスリープかけて削除とか気にしない
	temp = strchr(file, '/');
	if( temp != NULL ) *temp = '\0';
	strcat(file, "/PICTURE");
	sceIoMkdir(file, 0777);
	strcat(file, "/capture");
	sceIoMkdir(file, 0777);

	// 現在の動作クロック保持
	// CFW側でいじってる場合は意味ないんだよね…
	int clockCPU = scePowerGetCpuClockFrequency();
	int clockBUS = scePowerGetBusClockFrequency();

	while( 1 )
	{
	   	sceCtrlPeekBufferPositive(&pad, 1);
		
		if((pad.Buttons != padLast) && ((pad.Buttons & buttons) == buttons)){

			// サスペンド用のメモリ領域を作業バッファに使うので電源スイッチをロック
			// ファイル書き込み中に電源切られるのも嫌だし
			scePowerLock(0);

			// 比較的重い処理なのでクロックうｐ
			scePowerSetClockFrequency(333,333,166);

			// 他スレッドをサスペンド
			while(!libmExecTCmd( LIBM_TCMD_SUSPEND ));

			sceRtcGetCurrentClockLocalTime(&rtime);
			sprintf(
				filename, "%s/%04d%02d%02d%02d%02d%02d." MODULE_TYPE,
				file, rtime.year, rtime.month, rtime.day, rtime.hour, rtime.minutes, rtime.seconds
			);

			screenshot(filename);

			// 他スレッドをレジューム
			// screenshot内でエラーがあった時用
			if( libmExecTCmd(LIBM_TCMD_GET) == LIBM_TCMD_SUSPEND ){
				while(!libmExecTCmd( LIBM_TCMD_RESUME ));
			}

			// 立つ鳥後を濁さず
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
	// スタックサイズてきとう
    SceUID main_thid = sceKernelCreateThread(MODULE_NAME "_Main", threadMain, 16,0x1500, 0, NULL);
	
    if(main_thid >= 0) sceKernelStartThread(main_thid, args, argp);
	
    return 0;
}

int module_stop(SceSize args, void *argp)
{
    return 0;
}
