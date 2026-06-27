#include "HitStop.h"
#include "../Time/Time.h"
#include <algorithm>

CHitStop::CHitStop()
{
	m_HitStopDataList =
	{
		{ EHitStopType::LOW, HitStopData(0.05f, 0.1f) },
		{ EHitStopType::MIDDLE, HitStopData(0.12f, 0.1f) },
		{ EHitStopType::HIGH, HitStopData(1.0f, 0.1f) },
	};
}

void CHitStop::Start(const EHitStopType Type)
{
	if (!m_HitStopDataList.contains(Type))
	{
		return;
	}
	m_IsActive = true;
	m_CurrentHitStopData = m_HitStopDataList[Type];
	m_PrevTime = std::chrono::steady_clock::now();
	if (CTime* time = CTime::GetInstance())
	{
		time->SetTimeScale(m_CurrentHitStopData.TimeScale);
	}
}

void CHitStop::Update()
{
	if (!m_IsActive)
	{
		return;
	}

	const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	std::chrono::duration<float> diff = now - m_PrevTime;
	m_PrevTime = now;

	// timeScale に依存しない実時間でカウントダウン
	m_CurrentHitStopData.HitStopTime -= diff.count();

	//ヒットストップ時間が終了したか
	if (m_CurrentHitStopData.HitStopTime <= 0.0)
	{
		// 終了: タイムスケールを復元（1.0f）
		if (CTime* time = CTime::GetInstance())
		{
			time->SetTimeScale(1.0f);
		}
		m_IsActive = false;
		m_CurrentHitStopData.HitStopTime = 0.0;
	}
	else
	{
		// 継続中は常にスケールを適用（念のため）
		if (CTime* time = CTime::GetInstance())
		{
			time->SetTimeScale(m_CurrentHitStopData.TimeScale);
		}
	}
}