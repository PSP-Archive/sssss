/*
	画像ライブラリ用自作malloc/free
	ものすっごい手抜き
*/

#include <stdbool.h>
#include <pspkerneltypes.h>
#include <pspsuspend.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "nidresolve.h"

#define MEMORY_PAGE_SIZE 256
#define MEMORY_VOLATILE 5
#define MEMORY_VOLATILE_ADDR 0x88400000
#define MEMORY_VOLATILE_SIZE 0x00400000

static int stMemId;
static void *stMemAddr;
static unsigned int stMemNow;
static int stMemSize = MEMORY_VOLATILE_SIZE /2; // 全確保の勇気が出ないのでなんとなく半分の2MB

// malloc用に大きめのメモリを確保しておく
bool mallocInit()
{
	nidResolve();

	stMemNow = 0;

	// スリープ用メモリ領域のロック
	// ロック解除方法調べたけどわからんので放置。第3引数に0xCで解除なのかな？
	// sceKernelVolatileMemTryLockだとMHP3のオンライン集会所なんかではロックできなかった気がする
	// まーそりゃロックされちゃってるよねー
	if (sceKernelSetDdrMemoryProtection((void*)MEMORY_VOLATILE_ADDR, MEMORY_VOLATILE_SIZE, 0xF) < 0) return false;

	// スリープ用メモリ領域からmalloc用にメモリをがっつり確保
	stMemId = sceKernelAllocPartitionMemory(MEMORY_VOLATILE, "my_malloc", PSP_SMEM_High, stMemSize, NULL);
	if (stMemId < 0) return false;

	stMemAddr = (void *)sceKernelGetBlockHeadAddr(stMemId);

	return stMemAddr? true : false;
}

// 後始末
void mallocEnd()
{
	nidResolve();

	sceKernelFreePartitionMemory(stMemId);
}

// 確保済メモリから必要分のみ切り分け
// つっても確保済みメモリのアドレス返してるだけ
void *malloc( SceSize size )
{
	// 妙なサイズ指定されるとヤバいのでページサイズに合わせる
	SceSize real_size = MEMORY_PAGE_SIZE * (unsigned int)ceil( (double)size / (double)MEMORY_PAGE_SIZE );

	if (stMemSize < (stMemNow + real_size)) return NULL;

	void *addr = (void *)(stMemAddr + stMemNow);

	stMemNow += real_size;

	return addr;
}

//完全ダミー
void free( void *memblock ){}
