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
//	sockaddr_in address;    /*客户端socket地址*/
//	int sockfd;   /*客户端套接字*/
//	char buffer[BUFFER_SIZE];    /*读缓冲区*/
//	util_timer* timer;    /*定时器*/
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
	UINT16					 m_Interval;              //单位毫秒
	CThreadMutex			 m_csTimerWheel;
	TimerWheel				 m_timerWheel;
};


#endif // !__TIMER_H__
