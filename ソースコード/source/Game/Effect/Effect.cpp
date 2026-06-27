#include "Effect.h"
#include "../../Framework/Polygon/Billboard/Billboard.h"
#include "../../Framework/TextureLoader/TextureLoader.h"
#include "../../Framework/System/SystemContext.h"
#include <random>
#include <chrono>
#include <thread>

namespace
{
	constexpr float CULLING_RADIUS = 1.0f;
};

CEffect::CEffect()
{
	m_Id = GenerateID();
	m_Billboard = std::make_unique<CBillboard>();
	if (m_Billboard)
	{
		m_Billboard->EffectInit();
	}
}

CEffect::~CEffect()
{
	if (m_Billboard)
	{
		m_Billboard.reset();
	}
}

UINT CEffect::GenerateID()
{
	static UINT counter = 0;
	return counter++;
}

UINT CEffect::Active(const EffectParameter& InParameter)
{
	m_EffectParameter = InParameter;
	IsActiveEffect = true;
	if (!m_Billboard)
	{
		return 0;
	}
	const ETextureType type = m_EffectParameter.GetTextureType();
	if (m_Billboard->GetTextureType() != type)
	{
		m_Billboard->SetTexture(type);
	}

	if (m_EffectParameter.IsRandomStartTime())
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> random(0.0f, m_EffectParameter.GetAnimationEndTime());
		m_EffectParameter.CountTime = random(mt);
	}
	return m_Id;
}

void CEffect::Update(float DeltaTime)
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	m_IsCulling = system->VFCulling(m_EffectParameter.Position, CULLING_RADIUS);

	m_EffectParameter.CountTime += DeltaTime;

	//アニメの終わりまで行ったら消去(ループONだったら一から再生)
	if (m_EffectParameter.IsEndTime())
	{
		if (m_EffectParameter.IsLoop())
		{
			m_EffectParameter.ResetCountTime();
		}
		else
		{
			Deactivate();
		}
	}
}

void CEffect::Draw()
{
	if (!m_Billboard)
	{
		return;
	}

	if (!m_IsCulling)
	{
		return;
	}
	
	m_Billboard->Draw(m_EffectParameter);
}

void CEffect::Deactivate()
{
	IsActiveEffect = false;
}

void CEffect::UpdatePosition(const Vector3& Position)
{
	m_EffectParameter.Position = Position;
}
