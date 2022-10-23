#include "TimerWheel.h"
#include "TemplateThreadPool.h"
//#include "ThreadTask.h"

#define FIRST_INDEX(v) ((v) & TWF_MASK)
#define NTH_INDEX(v,n) (((v)>>(TWF_BITS +(n)* TWN_BITS )) & TWN_MASK)


//TimerNode 管理相关
CThreadMutex  gbs_csTimerNode;
vector<TimerNode*>  gbs_TimerNodeList;
TThreadPool<> gbs_TimerThreadPool(30);

bool cmp(TimerNode* a, TimerNode* b)
{
	return a->uniqueID < b->uniqueID;
}

void InsertVectorInSort(vector<TimerNode*> & vec, TimerNode * newNode)
{
	gbs_csTimerNode.Lock();
	vector<TimerNode*>::iterator itor  = lower_bound(vec.begin(), vec.end(), newNode,cmp);
	vec.insert(itor, newNode);
	gbs_csTimerNode.UnLock();
	//sort(vec.begin(), vec.end());
}

TimerNode* FindTimerNodeInVector(vector<TimerNode*> & vec,  DWORD unique)
{
	TimerNode tempNode;
	tempNode.uniqueID = unique;
	gbs_csTimerNode.Lock();
	if (binary_search(vec.begin(), vec.end(), &tempNode, cmp))
	{
		vector<TimerNode*>::iterator itor = lower_bound(vec.begin(), vec.end(), &tempNode, cmp);
		gbs_csTimerNode.UnLock();
		return *itor;
	}
	else
	{
		gbs_csTimerNode.UnLock();
		return NULL;
	}

}

bool RemoveVectorInSort(vector<TimerNode*> & vec, TimerNode * newNode)
{
	gbs_csTimerNode.Lock();
	if (binary_search(vec.begin(),vec.end(), newNode, cmp))
	{
		vector<TimerNode*>::iterator itor = lower_bound(vec.begin(), vec.end(), newNode, cmp);
		TimerNode* pTimeNode = *itor;
		delete pTimeNode;
		pTimeNode = NULL;
		vec.erase(itor);
		gbs_csTimerNode.UnLock();
		return true;
	}
	else
	{
		gbs_csTimerNode.UnLock();
		return false;
	}
	
}


//TimerWheel相关

void TimerWheel_init(TimerWheel * tw, uint16_t interval, uint64_t currentTime)
{
	memset(tw, 0, sizeof(TimerWheel));
	tw->interval = interval;
	tw->lastTimeStamp = currentTime;
	int i, j;
	for (i = 0; i < TWF_SIZE; ++i)
	{
		CycleLinkList::Linklist_init(tw->twFirst.list + i);
	}

	
	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < TWN_SIZE; ++j)
		{
			CycleLinkList::Linklist_init(tw->twOther[i].list + j);
		}
	}

	gbs_TimerThreadPool.Start();
}

void TimerWheel_node_init(TimerNode * node, TimerCallback callback, void * userData)
{
	node->prev = NULL;
	node->next = NULL;
	node->userdata = userData;
	node->callback = callback;
	node->expire = 0;
}

static void _TimerWheel_add(TimerWheel * tw, TimerNode * node)
{
	UINT64 expire = node->expire;
	if (expire < tw->currentTick) return;
	UINT64 idx = expire - tw->currentTick;			//expire也是刻度，实际延迟时间/interval，idx换算成时间轮最小刻度的实际刻度
	linknode * head = NULL;
	if (idx < TWF_SIZE)
	{
		head = tw->twFirst.list + FIRST_INDEX(expire);		    //放在最小刻度上，准备执行
	}
	else
	{
		UINT64 compare_Val = 0;
		for (int i = 0; i < 4; ++i)								
		{	
			compare_Val = (UINT64)1 << (TWF_BITS + (i + 1) * TWN_BITS);   //判断高刻度轮上的位置
			if (compare_Val > idx)								//找到了对应的高刻度轮
			{
				idx = NTH_INDEX(expire, i);						
				head = tw->twOther[i].list + idx;				//标记好高刻度轮的刻度的位置 
				break;
			}
		}
	}
	CycleLinkList::Linklist_add_back(head, (linknode*)node);	//在刻度位置的循环列表中插入定时器节点
}

void TimerWheel_add(TimerWheel * tw, TimerNode * node, uint32_t ticks)
{
	node->expire = tw->currentTick  + ((ticks > 0) ? ticks : 1) ;
	_TimerWheel_add(tw, node);
}

int TimerWheel_del(TimerWheel * tw, TimerNode * node)
{
	if (!CycleLinkList::Linklist_is_empty((linknode*)node))
	{
		CycleLinkList::Linklist_remote((linknode*)node);
		return 1;
	}
	return 0;
}


void _TimerWheel_cascade(TimerWheel * tw, TWOther* twOther,  int idx)
{
	linknode head;
	CycleLinkList::Linklist_init(&head);
	CycleLinkList::Linklist_splice(twOther->list + idx, &head);
	while (!CycleLinkList::Linklist_is_empty(&head))
	{
		TimerNode* node = (TimerNode*)head.next;                //依次取得头节点后的第一个节点
		CycleLinkList::Linklist_remote(head.next);				//从链表中移除该节点
		_TimerWheel_add(tw,node);                               //将满足触发要求的节点放在低位轮,以便触发
	}
}


void _TimerWheel_tick(TimerWheel* tw)
{
	++tw->currentTick;									        //刻度加一

	UINT32 currentTick = tw->currentTick;
	int index = (currentTick & TWF_MASK);						//低刻度轮到0
	if (index == 0)
	{
		int i = 0;
		int idx;
		do {
			idx = NTH_INDEX(tw->currentTick, i);			    //判断高刻度对应刻度的定时器节点
			_TimerWheel_cascade(tw, &(tw->twOther[i]), idx);
		} while (idx == 0 && (++i < 4));
	}

	linknode head;
	CycleLinkList::Linklist_init(&head);
	CycleLinkList::Linklist_splice(tw->twFirst.list + index, &head);
	while (!CycleLinkList::Linklist_is_empty(&head))            //遍历低刻度轮对应刻度上的链表，执行回调方法
	{ 
		TimerNode * timerNode = (TimerNode*)head.next;
		CycleLinkList::Linklist_remote(head.next);
		if (timerNode->callback)
		{
			//可以加到其他多线程中，减少运行时间
			//timerNode->callback(timerNode->userdata);
			gbs_TimerThreadPool.AddTask((void*)timerNode->userdata, timerNode->dataLen, timerNode->callback);
		}
		///////////////////////////////////////
		//TODO:加个枚举，区分1、执行一次的定时器 2、需要循环间隔执行的定时器
		if (timerNode->dwTimerType == TimerType::Repeat)
		{
			TimerWheel_node_init(timerNode, timerNode->callback, timerNode->userdata);
			timerNode->expire = currentTick + timerNode->dwDueTime / tw->interval;
			_TimerWheel_add(tw, timerNode);
		}
		else {
			RemoveVectorInSort(gbs_TimerNodeList, timerNode);
		}
		//////////////////////////////////////
	}

}

void TimerWheel_update(TimerWheel * tw, UINT64 currentTime)
{
	if (currentTime > tw->lastTimeStamp)
	{
		UINT64 diff = currentTime - tw->lastTimeStamp+tw->remainder;
		int interval = tw->interval;
		tw->lastTimeStamp = currentTime;
		while (diff >= interval)                 //interval为低刻度每两个刻度之间的时间
		{
			diff -= interval;
			_TimerWheel_tick(tw);
		}
		tw->remainder = (UINT16)diff;
	}
}
