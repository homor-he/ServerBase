#include "TimerWheel.h"
#include "TemplateThreadPool.h"
//#include "ThreadTask.h"

#define FIRST_INDEX(v) ((v) & TWF_MASK)
#define NTH_INDEX(v,n) (((v)>>(TWF_BITS +(n)* TWN_BITS )) & TWN_MASK)


//TimerNode �������
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


//TimerWheel���

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
	UINT64 idx = expire - tw->currentTick;			//expireҲ�ǿ̶ȣ�ʵ���ӳ�ʱ��/interval��idx�����ʱ������С�̶ȵ�ʵ�ʿ̶�
	linknode * head = NULL;
	if (idx < TWF_SIZE)
	{
		head = tw->twFirst.list + FIRST_INDEX(expire);		    //������С�̶��ϣ�׼��ִ��
	}
	else
	{
		UINT64 compare_Val = 0;
		for (int i = 0; i < 4; ++i)								
		{	
			compare_Val = (UINT64)1 << (TWF_BITS + (i + 1) * TWN_BITS);   //�жϸ߿̶����ϵ�λ��
			if (compare_Val > idx)								//�ҵ��˶�Ӧ�ĸ߿̶���
			{
				idx = NTH_INDEX(expire, i);						
				head = tw->twOther[i].list + idx;				//��Ǻø߿̶��ֵĿ̶ȵ�λ�� 
				break;
			}
		}
	}
	CycleLinkList::Linklist_add_back(head, (linknode*)node);	//�ڿ̶�λ�õ�ѭ���б��в��붨ʱ���ڵ�
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
		TimerNode* node = (TimerNode*)head.next;                //����ȡ��ͷ�ڵ��ĵ�һ���ڵ�
		CycleLinkList::Linklist_remote(head.next);				//���������Ƴ��ýڵ�
		_TimerWheel_add(tw,node);                               //�����㴥��Ҫ��Ľڵ���ڵ�λ��,�Ա㴥��
	}
}


void _TimerWheel_tick(TimerWheel* tw)
{
	++tw->currentTick;									        //�̶ȼ�һ

	UINT32 currentTick = tw->currentTick;
	int index = (currentTick & TWF_MASK);						//�Ϳ̶��ֵ�0
	if (index == 0)
	{
		int i = 0;
		int idx;
		do {
			idx = NTH_INDEX(tw->currentTick, i);			    //�жϸ߿̶ȶ�Ӧ�̶ȵĶ�ʱ���ڵ�
			_TimerWheel_cascade(tw, &(tw->twOther[i]), idx);
		} while (idx == 0 && (++i < 4));
	}

	linknode head;
	CycleLinkList::Linklist_init(&head);
	CycleLinkList::Linklist_splice(tw->twFirst.list + index, &head);
	while (!CycleLinkList::Linklist_is_empty(&head))            //�����Ϳ̶��ֶ�Ӧ�̶��ϵ�����ִ�лص�����
	{ 
		TimerNode * timerNode = (TimerNode*)head.next;
		CycleLinkList::Linklist_remote(head.next);
		if (timerNode->callback)
		{
			//���Լӵ��������߳��У���������ʱ��
			//timerNode->callback(timerNode->userdata);
			gbs_TimerThreadPool.AddTask((void*)timerNode->userdata, timerNode->dataLen, timerNode->callback);
		}
		///////////////////////////////////////
		//TODO:�Ӹ�ö�٣�����1��ִ��һ�εĶ�ʱ�� 2����Ҫѭ�����ִ�еĶ�ʱ��
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
		while (diff >= interval)                 //intervalΪ�Ϳ̶�ÿ�����̶�֮���ʱ��
		{
			diff -= interval;
			_TimerWheel_tick(tw);
		}
		tw->remainder = (UINT16)diff;
	}
}
