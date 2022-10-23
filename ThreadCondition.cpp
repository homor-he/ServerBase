#include "ThreadCondition.h"

void CThreadCondition::Wait()
{
	WaitForSingleObject(_phEvent, INFINITE);

	ResetEvent(_phEvent);
}

void CThreadCondition::Signal()
{
	SetEvent(_phEvent);
}
