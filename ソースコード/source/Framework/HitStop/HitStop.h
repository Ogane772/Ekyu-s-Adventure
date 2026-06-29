#pragma once
//ヒットストップを行うクラス
#include "../Singleton/Singleton.h"
#include <chrono>
#include <unordered_map>

enum class EHitStopType
{
	LOW,
	MIDDLE,
	HIGH,
};

class CHitStop : public CSingleton<CHitStop>
{
private:
	struct HitStopData
	{
		//ヒットストップ時間
		float HitStopTime = 0.0;
		//ヒットストップ中の時間倍率
		float TimeScale = 0.0f;

		HitStopData() {};
		HitStopData(const float InHitStopTime, const float InTimeScale)
		{
			HitStopTime = InHitStopTime;
			TimeScale = InTimeScale;
		};
	};

	bool m_IsActive = false;
	std::chrono::steady_clock::time_point m_PrevTime;
	std::unordered_map<EHitStopType, HitStopData> m_HitStopDataList;
	HitStopData m_CurrentHitStopData;

public:
	
	CHitStop();
	void Start(const EHitStopType Type);
	void Update();
};