#ifndef __TIMERTASK_H__
#define __TIMERTASK_H__

#pragma once

#include "ThreadTask.h"
#include "ThreadMutex.h"
#include <windef.h>
#include "TimerWheel.h"
#include <sys/timeb.h>
//struct client_data
//{
//	sockaddr_in address;    /*�ͻ���socket��ַ*/
//	int sockfd;   /*�ͻ����׽���*/
//	char buffer[BUFFER_SIZE];    /*��������*/
//	util_timer* timer;    /*��ʱ��*/
//};
struct UserData
{
	DWORD dwUserID;
	int startTime;
	int endTime;
};

class TimerTask : public ThreadTask
{
public:
	TimerTask();
	~TimerTask();

	void excute(void* para);

	void OnInit();
	void AddTimer(UserData* userData, int dataLen, TimerCallback callback,DWORD delayTime,DWORD timerType = TimerType::Once, DWORD dwDueTime = 0);
	void KillTimer(UserData* userData);

private:
	UINT16					 m_Interval;              //��λ����
	CThreadMutex			 m_csTimerWheel;
	TimerWheel				 m_timerWheel;
};


#endif // !__TIMER_H__
