#pragma once
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <list>
#include "../Singleton/Singleton.h"

class CHitStop;

class CTime : public CSingleton<CTime>
{
private:
	DWORD m_PrevCountTime = 0;         // 以前の時刻（ミリ秒）
	float m_CountTime = 0.0f;		   //経過時間
	std::list<float> m_TimeLst;       // 時間リスト
	UINT m_SampleNum = 0;              // 移動平均計算時のデータ数
	float m_SumTimes = 0.0f;           // 共通部分の合計値

	float m_TimeScale = 1.0f; // 時間倍率

	float m_FPS = 0.0f;//フレーム毎の時間
	float m_DeltaTime = 0.0f;//TimeScaleを考慮したフレーム毎の時間

	int m_TargetFPS = 60; // この値のFPSにそろえる

	CHitStop* m_HitStop = nullptr;//ヒットストップ更新用にクラスを保持

	// FPSを更新
	void UpdateFPS(const float DiffTime);
	// サンプル数を変更
	void SetSampleNum(const unsigned int Sample);
	// 現在の差分時刻をミリ秒単位で取得
	DWORD GetCurrentDiffTime() const;

public:
	// サンプル数を設定( 多い方が精度が上がる )
	void Initialize(unsigned int smp);
	void Finalize();
	void SetTargetFPS(unsigned int fps);

	// FPS値を取得
	float GetFPS() const;
	// ゲーム起動してからのタイマーを取得
	float GetTime() const;
	// 想定している秒間の値を返す
	float GetFrameTime() const;
	// 秒間の値を返す
	float GetDeltaTime() const;
	//時間倍率を返す
	float GetTimeScale() const { return m_TimeScale; }
	// GetDeltaTime()で返す値の倍数を設定( 基本は 1.0f )
	void SetTimeScale(const float Scale);
	// Updateなどの間隔をそろえるための関門
	bool Update();
};