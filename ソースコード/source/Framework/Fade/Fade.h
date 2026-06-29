#pragma once
//フェードクラス 通常のフェードと円形のフェードを行う
#include "../Singleton/Singleton.h"
#include "../Polygon/2DPolygon/2DPolygon.h"
#include <memory>
#include <vector>

class CFade : public CSingleton<CFade>
{
private:
	struct FadeParameter
	{
		float Alpha = 0.0f;//αの値
		float CountTime = 0.0f;//フェード開始からの経過時間を格納
		float FadeTime = 0.0f;//フェードを行うまでの時間
		bool IsFadeEnd = true;//フェードが終了したか
		bool IsFadePlay = false;//フェードを行っている状態か
		std::unique_ptr<C2DPolygon> FadePolygon;

		FadeParameter() {};
		void Init(const bool InIsFadeOut, const float InFadeTime)
		{
			FadeTime = InFadeTime;
			if (FadeTime == 0.0f)
			{
				IsFadeEnd = true;
				return;
			}
			IsFadePlay = true;
			IsFadeEnd = false;
			CountTime = 0.0f;

			if (InIsFadeOut)
			{
				Alpha = 0.0f;
			}
			else
			{
				Alpha = 1.0f;
			}
		}
	};

	std::vector<FadeParameter> m_FadeList;
	SpriteAnimationParameter m_CircleFadeParameter;
	bool m_IsFade = false;//通常のフェードと円フェードどちらかがフェードしているか
	bool m_IsFadeOut = false;//フェードインかフェードアウト（falseならフェードイン）
	bool m_IsFadeInWait = false;//シーン遷移後のフェードイン等、DeltaTimeが安定しない場合があるため一定時間フェードインアニメーションを待つための変数
	float m_FadeInWaitTime = 0.0f;//フェードインアニメーションを待つ時間

public:
	CFade();
	~CFade();
	void Start(const bool IsFadeOut, const float NormalFadeTime, const float CircleFadeTime);
	void Update(float DeltaTime);
	void Draw();
	bool IsFade() const { return m_IsFade; }
};