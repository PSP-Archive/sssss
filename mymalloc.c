/*
	�摜���C�u�����p����malloc/free
	���̂��������蔲��
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
static int stMemSize = MEMORY_VOLATILE_SIZE /2; // �S�m�ۂ̗E�C���o�Ȃ��̂łȂ�ƂȂ�������2MB

// malloc�p�ɑ傫�߂̃��������m�ۂ��Ă���
bool mallocInit()
{
	nidResolve();

	stMemNow = 0;

	// �X���[�v�p�������̈�̃��b�N
	// ���b�N�������@���ׂ����ǂ킩���̂ŕ��u�B��3������0xC�ŉ����Ȃ̂��ȁH
	// sceKernelVolatileMemTryLock����MHP3�̃I�����C���W��Ȃ񂩂ł̓��b�N�ł��Ȃ������C������
	// �܁[����Ⴭ�b�N���ꂿ����Ă��ˁ[
	if (sceKernelSetDdrMemoryProtection((void*)MEMORY_VOLATILE_ADDR, MEMORY_VOLATILE_SIZE, 0xF) < 0) return false;

	// �X���[�v�p�������̈悩��malloc�p�Ƀ�������������m��
	stMemId = sceKernelAllocPartitionMemory(MEMORY_VOLATILE, "my_malloc", PSP_SMEM_High, stMemSize, NULL);
	if (stMemId < 0) return false;

	stMemAddr = (void *)sceKernelGetBlockHeadAddr(stMemId);

	return stMemAddr? true : false;
}

// ��n��
void mallocEnd()
{
	nidResolve();

	sceKernelFreePartitionMemory(stMemId);
}

// �m�ۍσ���������K�v���̂ݐ؂蕪��
// ���Ă��m�ۍς݃������̃A�h���X�Ԃ��Ă邾��
void *malloc( SceSize size )
{
	// ���ȃT�C�Y�w�肳���ƃ��o���̂Ńy�[�W�T�C�Y�ɍ��킹��
	SceSize real_size = MEMORY_PAGE_SIZE * (unsigned int)ceil( (double)size / (double)MEMORY_PAGE_SIZE );

	if (stMemSize < (stMemNow + real_size)) return NULL;

	void *addr = (void *)(stMemAddr + stMemNow);

	stMemNow += real_size;

	return addr;
}

//���S�_�~�[
void free( void *memblock ){}
