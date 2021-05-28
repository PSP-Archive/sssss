#ifndef __NID_RESOLVE_H_
#define __NID_RESOLVE_H_

#include <pspctrl.h>
#include <systemctrl.h>
#include <psprtc.h>

#if 0

//ñ≥óùÇ‚ÇËÉ]ÉãÉo

extern int (*sceKernelDevkitVersion_Real)(void);

extern int (*sceKernelAllocPartitionMemory_Real)(SceUID id, const char *name, int type, SceSize size, void *addr);
extern void *(*sceKernelGetBlockHeadAddr_Real)(SceUID blockid);
extern int (*sceKernelFreePartitionMemory_Real)(SceUID blockid);

extern int (*sceKernelSetDdrMemoryProtection_Real)(void *addr, int size, int prot);

extern int (*sceRtcGetCurrentClockLocalTime_Real)(pspTime *time);

extern int (*sceCtrlPeekBufferPositive_Real)(SceCtrlData *pad_data, int count);
extern int (*sceCtrlSetSamplingCycle_Real)(int cycle);
extern int (*sceCtrlSetSamplingMode_Real)(int mode);

extern int (*sceDisplayGetFrameBuf_Real)(void **topaddr, int *bufferwidth, int *pixelformat, int sync);
extern int (*sceDisplayWaitVblankStart_Real)(void);


#define sceKernelDevkitVersion sceKernelDevkitVersion_Real

#define sceKernelAllocPartitionMemory sceKernelAllocPartitionMemory_Real
#define sceKernelGetBlockHeadAddr sceKernelGetBlockHeadAddr_Real
#define sceKernelFreePartitionMemory sceKernelFreePartitionMemory_Real

#define sceKernelSetDdrMemoryProtection sceKernelSetDdrMemoryProtection_Real

#define sceRtcGetCurrentClockLocalTime sceRtcGetCurrentClockLocalTime_Real

#define sceCtrlPeekBufferPositive sceCtrlPeekBufferPositive_Real
#define sceCtrlSetSamplingCycle sceCtrlSetSamplingCycle_Real
#define sceCtrlSetSamplingMode sceCtrlSetSamplingMode_Real

#define sceDisplayGetFrameBuf sceDisplayGetFrameBuf_Real
#define sceDisplayWaitVblankStart sceDisplayWaitVblankStart_Real

void nidResolve(void);

#else
#define nidResolve() {}
#endif

#endif
