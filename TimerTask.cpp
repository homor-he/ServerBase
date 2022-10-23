#include "TimerTask.h"
#include "inaddr.h"

UINT64 getTimeStampInMillion()
{
	timeb t;
	ftime(&t);
	return (UINT64)(t.time * 1000 + t.millitm);
}

TimerTask::TimerTask()
{
	OnInit();
}

TimerTask::~TimerTask()
{
}

void TimerTask::excute(void * para)
{
	//for (int i = 0; i < 10; ++i)
	//{
	//	cout << "timerTask" << i << endl;
	//}
	
	while (!_quit)
	{
		m_csTimerWheel.Lock();
		TimerWheel_update(&m_timerWheel, getTimeStampInMillion());
		m_csTimerWheel.UnLock();
	}
}

void TimerTask::OnInit()
{
	m_Interval = 5;
	TimerWheel_init(&m_timerWheel, m_Interval, getTimeStampInMillion());
}

void TimerTask::AddTimer(UserData* userData, int dataLen, TimerCallback callback, DWORD delayTime, DWORD timerType, DWORD dwDueTime)
{
	TimerNode* node = new TimerNode;
	node->uniqueID = userData->dwUserID;
	node->dwTimerType = timerType;
	node->dwDelayTime = delayTime;
	node->dwDueTime = dwDueTime;
	node->dataLen = dataLen;
	TimerWheel_node_init(node, callback, userData);
	m_csTimerWheel.Lock();
	TimerWheel_add(&m_timerWheel, node, delayTime / m_Interval + dwDueTime / m_Interval);
	m_csTimerWheel.UnLock();
	//m_TimerNodeList.push_back()
	InsertVectorInSort(gbs_TimerNodeList, node);
	
	for (size_t i = 0; i < gbs_TimerNodeList.size(); ++i)
	{
		TimerNode* pNode = gbs_TimerNodeList.at(i);
		cout << "uniqueId:" << pNode->uniqueID << "index:" << i << endl;
	}
}

void TimerTask::KillTimer(UserData * userData)
{
	TimerNode* pTimerNode = FindTimerNodeInVector(gbs_TimerNodeList, userData->dwUserID);
	if (NULL != pTimerNode)
	{
		m_csTimerWheel.Lock();
		TimerWheel_del(&m_timerWheel, pTimerNode);
		m_csTimerWheel.UnLock();
		RemoveVectorInSort(gbs_TimerNodeList, pTimerNode);
	}
}


