#include "Fade.h"
#include "../Renderer/Renderer.h"
#include "../TextureLoader/TextureLoader.h"
#include "../Sound/SoundManager.h"
#include "../Sound/SoundTypes.h"
#include <algorithm>

namespace
{
	constexpr int FADE_NUM = 3;
	constexpr int NORMAL_FADE_NUMBER = 0;
	constexpr int CIRCLE_FADE_OUT_NUMBER = 1;
	constexpr int CIRCLE_FADE_IN_NUMBER = 2;
	constexpr float CIRCLE_FADE_SIZE_WIDTH = 32.0f;
	constexpr float CIRCLE_FADE_SIZE_HEIGHT = 18.0f;
	constexpr int CIRCLE_FADE_WIDTH_PATTERN = 5;
	constexpr int CIRCLE_FADE_HEIGHT_PATTERN = 7;
	constexpr float WAIT_FADEIN_TIME = 1.4f;
};

CFade::CFade()
{
	m_FadeList.reserve(FADE_NUM);
	for (int i = 0; i < FADE_NUM; i++)
	{
		m_FadeList.emplace_back(FadeParameter());
	}
	m_FadeList[NORMAL_FADE_NUMBER].FadePolygon = std::make_unique<C2DPolygon>(ETextureType::BLACK);
	m_FadeList[CIRCLE_FADE_OUT_NUMBER].FadePolygon = std::make_unique<C2DPolygon>(ETextureType::CIRCLE_FADEOUT);
	m_FadeList[CIRCLE_FADE_IN_NUMBER].FadePolygon = std::make_unique<C2DPolygon>(ETextureType::CIRCLE_FADEIN);
	m_CircleFadeParameter.Init(Vector2(CIRCLE_FADE_SIZE_WIDTH, CIRCLE_FADE_SIZE_HEIGHT), CIRCLE_FADE_WIDTH_PATTERN, CIRCLE_FADE_HEIGHT_PATTERN);
}

CFade::~CFade()
{
	for (FadeParameter& parameter : m_FadeList)
	{
		parameter.FadePolygon.reset();
	}
}

void CFade::Start(const bool IsFadeOut, const float NormalFadeTime, const float CircleFadeTime)
{
	m_IsFadeOut = IsFadeOut;
	m_FadeList[NORMAL_FADE_NUMBER].Init(IsFadeOut, NormalFadeTime);
	//円フェードの場合はテクスチャが分かれているため、フェードに合う要素のみ代入
	for (int i = CIRCLE_FADE_OUT_NUMBER; i < FADE_NUM; i++)
	{
		m_FadeList[i].IsFadePlay = false;
		if (IsFadeOut && i == CIRCLE_FADE_OUT_NUMBER)
		{
			m_FadeList[i].Init(IsFadeOut, CircleFadeTime);
			CSoundManager* instance = CSoundManager::GetInstance();
			if (instance)
			{
				instance->Play(ESoundIndex::FADEOUT);
			}
		}
		else if(!IsFadeOut && i == CIRCLE_FADE_IN_NUMBER)
		{
			m_FadeList[i].Init(IsFadeOut, CircleFadeTime);
		}
	}
	if (!m_IsFadeOut)
	{
		m_IsFadeInWait = true;
		m_FadeInWaitTime = WAIT_FADEIN_TIME;
	}
	m_IsFade = true;
}

void CFade::Update(float DeltaTime)
{
	if (!m_IsFade)
	{
		return;
	}
	//シーン遷移後のフェードイン等、DeltaTimeが安定しない場合があるため一定時間フェードを更新しない
	if (m_IsFadeInWait)
	{
		m_FadeInWaitTime -= DeltaTime;
		if (m_FadeInWaitTime < 0.0f)
		{
			m_IsFadeInWait = false;
		}
		else
		{
			return;
		}
	}
	m_IsFade = false;
	for (FadeParameter& parameter : m_FadeList)
	{
		if (!parameter.IsFadePlay)
		{
			continue;
		}
		if (parameter.IsFadeEnd)
		{
			continue;
		}
	
		parameter.CountTime += DeltaTime;
		const float alpha = std::clamp(parameter.CountTime / parameter.FadeTime, 0.0f, 1.0f);

		if (alpha == 1.0f)
		{
			parameter.IsFadeEnd = true;
		}
		else
		{
			m_IsFade = true;
		}

		if (m_IsFadeOut)
		{
			parameter.Alpha = alpha;
		}
		else
		{
			//フェードインの場合は徐々に暗くなるため1から引く
			parameter.Alpha = 1.0f - alpha;
		}
	}
}

void CFade::Draw()
{
	//フェードインの終了後は画面が明けた状態であるため描画を行わない
	if (!m_IsFade && !m_IsFadeOut)
	{
		return;
	}
	CRenderer::Set2DShader();
	CRenderer::SetWorldViewProjection2D();
	int count = 0;
	for (FadeParameter& parameter : m_FadeList)
	{
		if (!parameter.FadePolygon)
		{
			count++;
			continue;
		}
		if (!parameter.IsFadePlay)
		{
			count++;
			continue;
		}
		//円フェードの場合はアニメーションテクスチャなのでアニメーション用の描画関数を呼ぶ
		if (count >= CIRCLE_FADE_OUT_NUMBER)
		{
			if (m_IsFadeInWait)
			{
				continue;
			}
			CTextureLoader* textureLoader = CTextureLoader::GetInstance();
			if (!textureLoader)
			{
				count++;
				continue;
			}
			m_CircleFadeParameter.SetDrawParameter(Vector2::Zero(), parameter.CountTime, parameter.FadeTime);
			parameter.FadePolygon->DrawAnimation(m_CircleFadeParameter);
		}
		else
		{
			parameter.FadePolygon->Draw(parameter.Alpha);
		}
		count++;
	}
}