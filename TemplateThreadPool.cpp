#include "TemplateThreadPool.h"

//template <class T = ThreadTask>
//TThreadPool<T>::TThreadPool(int numThreads)
//{
//	_numThreads = numThreads;
//	_started = false;
//	_next = 0;
//	if (numThreads <= 0)
//	{
//		cout << "numThreads_<=0" << endl;
//		abort();							  //异常终止该进程
//	}
//}
//
//template <class T>
//TThreadPool<T>::~TThreadPool()
//{
//	for (int i = 0; i < _threads.size(); ++i)
//	{
//		delete _threads[i];
//	}
//
//	vector<Thread*>().swap(_threads);
//}
//
//template <class T>
//void TThreadPool<T>::Start()
//{
//	_started = true;
//	for (int i = 0; i < _numThreads; ++i)
//	{
//		string name = "EventLoopThread" + to_string(i + 1);
//		Thread* t = new Thread(name, i + 1);
//		//t->SetThreadFunc(&EventLoopThread::threadFunc);
//		_threads.push_back(t);
//		//_loops.push_back(loop);
//		t->start();
//	}
//}
//
//template <class T>
//void TThreadPool<T>::Start(function<void(void*)> CallBack)
//{
//	/*if (func == nullptr)
//	return;*/
//	funcCallBack = CallBack;
//	Start();
//}
//
//template <class T>
//void TThreadPool<T>::AddTask(void * para, int len)
//{
//	for (int i = _next; i < _numThreads; ++i)
//	{
//		Thread* pThread = _threads.at(i);
//		if (pThread->getJoinStatu())					//如果是阻塞状态,即task为空
//		{
//			T* pTask = new T(para, len);
//			pTask->setTaskFuncCallBack(funcCallBack);
//			pThread->SetTask(pTask);
//			cout << "threadID:" << pThread->getThreadID() << endl;
//			_next++;
//			_next = _next%_numThreads;
//			return;
//		}
//		else
//		{
//			_next++;
//			_next = _next%_numThreads;
//			if (!pThread->getStartedStatus())			//如果线程没有开始或已运行结束
//			{
//				T* pTask = pThread->GetTask();
//				if (pTask == NULL) return;
//				pTask->setPara(para, len);
//				pThread->Run();
//				return;
//			}
//		}
//	}
//
//	//如果线程都忙碌的话可以考虑动态扩容
//	string name = "EventLoopThread" + to_string(_numThreads);
//	Thread* t = new Thread(name, _numThreads, true);
//	t->start();
//	T* pTask = new T(para, len);
//	pTask->setTaskFuncCallBack(funcCallBack);
//	t->SetTask(pTask);
//	cout << "extraThread" << endl;
//}
