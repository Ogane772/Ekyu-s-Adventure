#include "Time.h"
#include "../HitStop/HitStop.h"

void CTime::Initialize(unsigned int smp)
{
	// サンプル数の設定
	SetSampleNum(smp);

	// 精度を上げる
	timeBeginPeriod(1);

	// 生成時の時刻（ミリ秒）を取得
	m_PrevCountTime = timeGetTime();

	// 計測
	UpdateFPS(static_cast<float>(GetCurrentDiffTime()));

	m_CountTime = 0.0;

	m_HitStop = CHitStop::GetInstance();
}

void CTime::Finalize()
{
	timeEndPeriod(1);    // タイマーの精度を戻す
}

void CTime::SetTargetFPS(unsigned int fps)
{
	m_TargetFPS = fps;
}

float CTime::GetFPS() const
{
	return m_FPS;
}

float CTime::GetTime() const
{
	return m_CountTime;
}

float CTime::GetFrameTime()const
{
	return 1.0f / m_TargetFPS;
}

bool CTime::Update()
{
	if (m_HitStop)
	{
		m_HitStop->Update();
	}
	const DWORD currentTime = timeGetTime();
	const DWORD currentDiffTime = currentTime - m_PrevCountTime;         // 差分カウント数を算出する

	if (currentDiffTime >= (DWORD)(1000.0f / m_TargetFPS))
	{
		UpdateFPS(static_cast<float>(currentDiffTime));
		m_CountTime += GetDeltaTime();
		m_PrevCountTime = currentTime; // 現在の時刻を保持
		return true;
	}
	return false;
}

// m_FPSを更新
void CTime::UpdateFPS(const float DiffTime)
{
	if (DiffTime == 0)
	{
		// 計算できないのでを返す
		return;
	}

	// 最も古いデータを消去
	m_TimeLst.pop_front();

	// 新しいデータを追加
	m_TimeLst.emplace_back(DiffTime);

	// 共通加算部分の更新
	m_SumTimes += DiffTime - *m_TimeLst.begin();

	// m_FPS算出
	const float aveDef = (m_SumTimes + DiffTime) / m_SampleNum;
	if (aveDef != 0)
	{
		m_FPS = 1000.0f / aveDef;
		if (m_FPS != 0.0)
		{
			m_DeltaTime = (float)(1.0 / m_FPS) * m_TimeScale;
		}
	}
}

DWORD CTime::GetCurrentDiffTime() const
{
	const DWORD currentTime = timeGetTime();
	const DWORD currentDiffTime = currentTime - m_PrevCountTime;         // 差分カウント数を算出する
	return currentDiffTime;
}

// サンプル数を変更
void CTime::SetSampleNum(const unsigned int Sample)
{
	m_SampleNum = Sample;    // 平均を計算する個数
	m_TimeLst.resize(m_SampleNum, 0.0);    // リスト初期化
	m_SumTimes = 0;
}

float CTime::GetDeltaTime() const
{
	return m_DeltaTime;
}

void CTime::SetTimeScale(const float Scale)
{
	m_TimeScale = Scale;
}
