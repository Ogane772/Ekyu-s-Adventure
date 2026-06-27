#include "TimeScheduler.h"

void CTimeScheduler::Run(float DeltaTime)
{
	if (m_Tasks.empty())
	{
		return;
	}

	if (IsEndTask())
	{
		return;
	}

	std::pair<float, std::function<void()>>& task = m_Tasks[m_CurrentIndex];

	const float playTime = task.first;
	if (m_CountTime >= playTime)
	{
		task.second();
		m_CurrentIndex++;
	}

	m_CountTime += DeltaTime;
}
