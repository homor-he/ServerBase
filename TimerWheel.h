#ifndef __TIMERWHEEL_H__
#define __TIMERWHEEL_H__

#pragma once

#include "TimerDefine.h"
#include "CycleLinkList.h"
#include <time.h>
#include <vector>
#include <algorithm>
#include "ThreadMutex.h"

extern CThreadMutex  gbs_csTimerNode;
extern vector<TimerNode*>  gbs_TimerNodeList;
//extern TThreadPool<ThreadTask> gbs_TimerThreadPool;

void InsertVectorInSort(vector<TimerNode*> & vec, TimerNode * newNode);
bool RemoveVectorInSort(vector<TimerNode*> & vec, TimerNode * newNode);
TimerNode* FindTimerNodeInVector(vector<TimerNode*> & vec, DWORD unique);

// 初始化时间轮，interval为每帧的间隔，currentTime为当前时间
void TimerWheel_init(TimerWheel *tw, uint16_t interval, uint64_t currentTime);

// 初始化时间结点：callback为回调，userData为用户数据
void TimerWheel_node_init(TimerNode *node, TimerCallback callback, void *userData);

// 增加时间结点，ticks为触发间隔(注意是以interval为单位)
void TimerWheel_add(TimerWheel *tw, TimerNode *node, uint32_t ticks);

// 删除结点
int TimerWheel_del(TimerWheel *tw, TimerNode *node);

// 更新时间轮
void TimerWheel_update(TimerWheel *tw, UINT64 currentTime);


#endif // !__TIMERWHEEL_H__
