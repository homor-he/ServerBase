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

// ��ʼ��ʱ���֣�intervalΪÿ֡�ļ����currentTimeΪ��ǰʱ��
void TimerWheel_init(TimerWheel *tw, uint16_t interval, uint64_t currentTime);

// ��ʼ��ʱ���㣺callbackΪ�ص���userDataΪ�û�����
void TimerWheel_node_init(TimerNode *node, TimerCallback callback, void *userData);

// ����ʱ���㣬ticksΪ�������(ע������intervalΪ��λ)
void TimerWheel_add(TimerWheel *tw, TimerNode *node, uint32_t ticks);

// ɾ�����
int TimerWheel_del(TimerWheel *tw, TimerNode *node);

// ����ʱ����
void TimerWheel_update(TimerWheel *tw, UINT64 currentTime);


#endif // !__TIMERWHEEL_H__
