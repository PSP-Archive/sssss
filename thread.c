#include "thread.h"

//プラグイン以外の他スレッドを停止・再開させる
int libmExecTCmd( int cmd )
{
	static int thid_status = LIBM_TCMD_RESUME;
	static bool thid_IsDump = false;
	static bool this_IsBusy = false;
	
	static int thid_list_first[MAX_NUMBER_OF_THREADS];
	static int thid_count_first;
	
	static int thid_list_Current[MAX_NUMBER_OF_THREADS];
	static int thid_count_Current;
	
	if( cmd ==  LIBM_TCMD_DUMP )
	{
		thid_IsDump = sceKernelGetThreadmanIdList( SCE_KERNEL_TMID_Thread, thid_list_first, MAX_NUMBER_OF_THREADS, &thid_count_first ) >= 0;
		
		return thid_IsDump;
	}
	else if( (cmd == LIBM_TCMD_SUSPEND || cmd == LIBM_TCMD_RESUME) && thid_IsDump && cmd != thid_status && !this_IsBusy )
	{
		thid_status = cmd;
		this_IsBusy = true;
		
		int i, j;
		int (*func)(SceUID) =  ( cmd == LIBM_TCMD_SUSPEND ? sceKernelSuspendThread : sceKernelResumeThread );
		SceKernelThreadInfo thinfo;
		
		
		if( cmd == LIBM_TCMD_SUSPEND )
		{
			//停止
			
			int Current_id	= sceKernelGetThreadId();
			
			//現在のスレッドリストを作る
			if( sceKernelGetThreadmanIdList( SCE_KERNEL_TMID_Thread, thid_list_Current, MAX_NUMBER_OF_THREADS, &thid_count_Current ) < 0 )
			{
				//現在のスレッドリスト取得失敗
				this_IsBusy = false;
				return 0;
			}
			
			
			//現在のスレッド一覧と、プラグイン起動時にダンプしたセーフリストを比較
			//セーフリストと現スレッドに一致するものは対象外へ
			for( i = 0; i < thid_count_Current; i++ )
			{
				memset(&thinfo, 0, sizeof(SceKernelThreadInfo));
				thinfo.size = sizeof(SceKernelThreadInfo);
				sceKernelReferThreadStatus(thid_list_Current[i], &thinfo);
				
				if
				(
				//	( strcasecmp( "SceImpose",thinfo.name) == 0 )
				//||
					( thinfo.attr & 0x100000 ) == 0x100000
				||
					( thid_list_Current[i] == Current_id )
				)
				{
					thid_list_Current[i] = 0;
					continue;
				}
				
				for( j = 0; j < thid_count_first; j++ )
				{
					if( thid_list_Current[i] == thid_list_first[j] )
					{
						thid_list_Current[i] = 0;
						break;
					}
				}
			}
		}
		
		
		//最終的な現在のスレッドリストにあるスレッドへ停止・再開の操作
		for( i = 0; i < thid_count_Current; i++ )
		{
			if( thid_list_Current[i] ) ( func )( thid_list_Current[i] );
		}
		
		this_IsBusy = false;
		
		return 1;
	}
	else if( cmd == LIBM_TCMD_GET )
	{
		return thid_status;
	}
	
	return 0;
}
