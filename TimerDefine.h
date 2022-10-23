#ifndef __TIMERDEFINE_H__
#define __TIMERDEFINE_H__

#pragma once

#include "CycleLinkList.h"
#include "WinDef.h"
#include <functional>

using namespace std;

// 第1个轮占的位数
#define TWF_BITS 8
// 第1个轮的长度
#define TWF_SIZE (1 << TWF_BITS)
// 第n个轮占的位数
#define TWN_BITS 6
// 第n个轮的长度
#define TWN_SIZE (1 << TWN_BITS)
// 掩码：取模或整除用
#define TWF_MASK (TWF_SIZE - 1)
#define TWN_MASK (TWN_SIZE - 1)

enum TimerType
{
	Once,			//执行一次
	Repeat,
};

typedef function<void(void*)> TimerCallback;

// 第1个轮
typedef struct TWFirst {
	linknode list[TWF_SIZE];
} TWFIRST,*PTWFIRST;

// 后面几个轮
typedef struct TWOther {
	linknode list[TWN_SIZE];
} TWOTHER,*PTWOTHER;

// 时间轮定时器
typedef struct TimerWheel {
	TWFirst twFirst;             // 第1个轮
	TWOther twOther[4];          // 后面4个轮
	UINT64 lastTimeStamp;        // 上一次调用时的时间 单位：毫秒
	UINT32 currentTick;          // 当前的tick
	UINT16 interval;             // 每个时间点的毫秒间隔(每个刻度的毫秒间隔)
	UINT16 remainder;            // 剩余的毫秒
}TIMERWHEEL, *PTIMEWHEEL;


// 定时器结点
typedef struct TimerNode {
	struct linknode *next;       // 下一个结点
	struct linknode *prev;       // 上一个结点
	void *userdata;              // 用户数据
	int dataLen;				 // 用户数据长度
	TimerCallback callback;      // 回调函数
	UINT32 expire;               // 到期时间
	UINT32 uniqueID;			 // 定时器节点标识
	DWORD dwTimerType;			 // 定时器类型 （区分是延时执行一次的定时器还是循环触发的定时器）
	UINT32 dwDelayTime;			 // 用户传入的延时时间
	UINT32 dwDueTime;			 // 第一次调用的延时时间
	//bool operator<(const TimerNode* m)const { //定义比较方式，这一步很重要
	//	return this->uniqueID < m->uniqueID;
	//}

	//bool operator<(const TimerNode m)const { //定义比较方式，这一步很重要
	//	return uniqueID < m.uniqueID;
	//}

} TIMERNODE, *PTIMERNODE;


#endif // !__TIMERDEFINE_H__
