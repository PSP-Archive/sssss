#include <pspsdk.h>
#include <pspkernel.h>
#include <systemctrl.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspsysmem_kernel.h>
#include <psprtc.h>

#include <stdbool.h>

#define PSP_FIRMWARE(f) ((((f >> 8) & 0xF) << 24) | (((f >> 4) & 0xF) << 16) | ((f & 0xF) << 8) | 0x10)

int (*sceKernelDevkitVersion_Real)(void);

int (*sceKernelAllocPartitionMemory_Real)(SceUID id, const char *name, int type, SceSize size, void *addr);
void *(*sceKernelGetBlockHeadAddr_Real)(SceUID blockid);
int (*sceKernelFreePartitionMemory_Real)(SceUID blockid);

int (*sceKernelSetDdrMemoryProtection_Real)(void *addr, int size, int prot);

int (*sceRtcGetCurrentClockLocalTime_Real)(pspTime *time);

int (*sceCtrlPeekBufferPositive_Real)(SceCtrlData *pad_data, int count);
int (*sceCtrlSetSamplingCycle_Real)(int cycle);
int (*sceCtrlSetSamplingMode_Real)(int mode);

int (*sceDisplayGetFrameBuf_Real)(void **topaddr, int *bufferwidth, int *pixelformat, int sync);
int (*sceDisplayWaitVblankStart_Real)(void);

static bool alreadyResolved = false;


void nidResolve(void)
{

	if( alreadyResolved ) return;

	sceKernelDevkitVersion_Real = (void *)FindProc("sceSystemMemoryManager", "SysMemUserForUser", 0x3FC9AE6A);

	if( sceKernelDevkitVersion_Real() >= PSP_FIRMWARE(0x635) ){

		sceKernelAllocPartitionMemory_Real		= (void *)FindProc("sceSystemMemoryManager", "SysMemForKernel", 0x4621A9CC);
		sceKernelGetBlockHeadAddr_Real			= (void *)FindProc("sceSystemMemoryManager", "SysMemForKernel", 0x52B54B93);
		sceKernelFreePartitionMemory_Real		= (void *)FindProc("sceSystemMemoryManager", "SysMemForKernel", 0x8FDAFC4C);

		sceKernelSetDdrMemoryProtection_Real	= (void *)FindProc("sceSystemMemoryManager", "SysMemForKernel", 0x00E9A04A);

		sceRtcGetCurrentClockLocalTime_Real		= (void *)FindProc("sceRTC_Service", "sceRtc_driver", 0x0287B1C2);

		sceCtrlPeekBufferPositive_Real			= (void *)FindProc("sceController_Service", "sceCtrl_driver", 0x18654FC0);
		sceCtrlSetSamplingCycle_Real			= (void *)FindProc("sceController_Service", "sceCtrl_driver", 0x855C255D);
		sceCtrlSetSamplingMode_Real				= (void *)FindProc("sceController_Service", "sceCtrl_driver", 0x6CB49301);

		sceDisplayGetFrameBuf_Real				= (void *)FindProc("sceDisplay_Service", "sceDisplay_driver", 0x08A10838);
		sceDisplayWaitVblankStart_Real			= (void *)FindProc("sceDisplay_Service", "sceDisplay_driver", 0xC30D327D);
	}
	else {

		sceKernelAllocPartitionMemory_Real		= sceKernelAllocPartitionMemory;
		sceKernelGetBlockHeadAddr_Real			= sceKernelGetBlockHeadAddr;
		sceKernelFreePartitionMemory_Real		= sceKernelFreePartitionMemory;

		sceKernelSetDdrMemoryProtection_Real	= sceKernelSetDdrMemoryProtection;

		sceRtcGetCurrentClockLocalTime_Real		= sceRtcGetCurrentClockLocalTime;

		sceCtrlPeekBufferPositive_Real			= sceCtrlPeekBufferPositive;
		sceCtrlSetSamplingCycle_Real			= sceCtrlSetSamplingCycle;
		sceCtrlSetSamplingMode_Real				= sceCtrlSetSamplingMode;

		sceDisplayGetFrameBuf_Real				= sceDisplayGetFrameBuf;
		sceDisplayWaitVblankStart_Real			= sceDisplayWaitVblankStart;
	}

	alreadyResolved = true;

}
