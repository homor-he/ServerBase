#ifndef __TIMERDEFINE_H__
#define __TIMERDEFINE_H__

#pragma once

#include "CycleLinkList.h"
#include "WinDef.h"
#include <functional>

using namespace std;

// ��1����ռ��λ��
#define TWF_BITS 8
// ��1���ֵĳ���
#define TWF_SIZE (1 << TWF_BITS)
// ��n����ռ��λ��
#define TWN_BITS 6
// ��n���ֵĳ���
#define TWN_SIZE (1 << TWN_BITS)
// ���룺ȡģ��������
#define TWF_MASK (TWF_SIZE - 1)
#define TWN_MASK (TWN_SIZE - 1)

enum TimerType
{
	Once,			//ִ��һ��
	Repeat,
};

typedef function<void(void*)> TimerCallback;

// ��1����
typedef struct TWFirst {
	linknode list[TWF_SIZE];
} TWFIRST,*PTWFIRST;

// ���漸����
typedef struct TWOther {
	linknode list[TWN_SIZE];
} TWOTHER,*PTWOTHER;

// ʱ���ֶ�ʱ��
typedef struct TimerWheel {
	TWFirst twFirst;             // ��1����
	TWOther twOther[4];          // ����4����
	UINT64 lastTimeStamp;        // ��һ�ε���ʱ��ʱ�� ��λ������
	UINT32 currentTick;          // ��ǰ��tick
	UINT16 interval;             // ÿ��ʱ���ĺ�����(ÿ���̶ȵĺ�����)
	UINT16 remainder;            // ʣ��ĺ���
}TIMERWHEEL, *PTIMEWHEEL;


// ��ʱ�����
typedef struct TimerNode {
	struct linknode *next;       // ��һ�����
	struct linknode *prev;       // ��һ�����
	void *userdata;              // �û�����
	int dataLen;				 // �û����ݳ���
	TimerCallback callback;      // �ص�����
	UINT32 expire;               // ����ʱ��
	UINT32 uniqueID;			 // ��ʱ���ڵ��ʶ
	DWORD dwTimerType;			 // ��ʱ������ ����������ʱִ��һ�εĶ�ʱ������ѭ�������Ķ�ʱ����
	UINT32 dwDelayTime;			 // �û��������ʱʱ��
	UINT32 dwDueTime;			 // ��һ�ε��õ���ʱʱ��
	//bool operator<(const TimerNode* m)const { //����ȽϷ�ʽ����һ������Ҫ
	//	return this->uniqueID < m->uniqueID;
	//}

	//bool operator<(const TimerNode m)const { //����ȽϷ�ʽ����һ������Ҫ
	//	return uniqueID < m.uniqueID;
	//}

} TIMERNODE, *PTIMERNODE;


#endif // !__TIMERDEFINE_H__
