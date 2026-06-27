#include "EffectManager.h"
#include "EffectTypes.h"
#include "Effect.h"
#include "../../Framework/TextureLoader/TextureTypes.h"
#include "../../Framework/Scene/SceneManager.h"

//フレーム数を秒に変換する
constexpr float ONE_FRAME_TIME = 1.0f / 60.0f;
#define CONVERT_ANIMATION_TIME(frame) frame * ONE_FRAME_TIME

namespace
{
	constexpr int POOL_OBJECT_NUM = 130;//エフェクトのプールオブジェクト数
};

CEffectManager::CEffectManager()
{
	m_EffectDataList =
	{
		{ EEffectType::ACORN,	EffectData(true, ETextureType::ACORN_ANIME, CONVERT_ANIMATION_TIME(10), 5, 1, false, true) },
		{ EEffectType::WALL_HIT,	EffectData(false, ETextureType::EFFECT_WALL_HIT, CONVERT_ANIMATION_TIME(4), 5, 1, true) },
		{ EEffectType::DASH,	EffectData(false, ETextureType::EFFECT_DASH, CONVERT_ANIMATION_TIME(2), 4, 2, true) },
		{ EEffectType::JUMP,	EffectData(false, ETextureType::EFFECT_JUMP, CONVERT_ANIMATION_TIME(3), 5, 2, true) },
		{ EEffectType::STUN,	EffectData(true, ETextureType::EFFECT_STUN, CONVERT_ANIMATION_TIME(6), 5, 2, false) },
		{ EEffectType::ITEM_GET,	EffectData(false, ETextureType::EFFECT_ITEM_GET, CONVERT_ANIMATION_TIME(5), 8, 1, true) },
		{ EEffectType::SMOKE,	EffectData(false, ETextureType::EFFECT_SMOKE, CONVERT_ANIMATION_TIME(5), 3, 1, true) },
		{ EEffectType::REVIVAL ,	EffectData(false, ETextureType::EFFECT_REVIVAL, CONVERT_ANIMATION_TIME(4), 5, 3, true) },
		{ EEffectType::BOM,	EffectData(true, ETextureType::BOM_ANIME, CONVERT_ANIMATION_TIME(4), 4, 1, true) },
		{ EEffectType::EXPLOSION,	EffectData(false, ETextureType::EFFECT_EXPLOSION, CONVERT_ANIMATION_TIME(3), 5, 3, true) },
		{ EEffectType::EXPLOSION2,	EffectData(false, ETextureType::EFFECT_EXPLOSION2, CONVERT_ANIMATION_TIME(4), 5, 4, true) },
		{ EEffectType::HEART,	EffectData(false, ETextureType::EFFECT_HEART, CONVERT_ANIMATION_TIME(10), 5, 1, true) },
		{ EEffectType::ATTACK_HIT,	EffectData(false, ETextureType::EFFECT_ATTACK_HIT, CONVERT_ANIMATION_TIME(8), 5, 2, true) },
		{ EEffectType::ATTACK_HIT2,	EffectData(false, ETextureType::EFFECT_ATTACK_HIT2, CONVERT_ANIMATION_TIME(3), 5, 3, true) },
		{ EEffectType::ATTACK_HIT3,	EffectData(false, ETextureType::EFFECT_ATTACK_HIT3, CONVERT_ANIMATION_TIME(5), 5, 3, true) },
		{ EEffectType::PLAYER_HIT,	EffectData(false, ETextureType::EFFECT_PLAYER_HIT, CONVERT_ANIMATION_TIME(5), 5, 1, true) },
		{ EEffectType::GROUND_HIT,	EffectData(false, ETextureType::EFFECT_GROUND_HIT, CONVERT_ANIMATION_TIME(5), 5, 3, false) },
	};
	m_EffectPoolList.reserve(POOL_OBJECT_NUM);
	for(int i = 0; i < POOL_OBJECT_NUM; i++)
	{
		m_EffectPoolList.emplace_back(std::make_unique<CEffect>());
	}
}

UINT CEffectManager::Active(const EEffectType Type, const Vector3 Position, const float Size)
{
	for (std::unique_ptr<CEffect>& effect : m_EffectPoolList)
	{
		if (!effect)
		{
			continue;
		}
		if (effect->IsActive())
		{
			continue;
		}
		if (!m_EffectDataList.contains(Type))
		{
			continue;
		}
		const EffectData& data = m_EffectDataList[Type];
		EffectParameter parameter;
		parameter.Position = Position;
		parameter.Size = Size;
		parameter.EffectData = data;
		return effect->Active(parameter);
	}
	return 0;
}

void CEffectManager::Update(float DeltaTime)
{
	for (std::unique_ptr<CEffect>& effect : m_EffectPoolList)
	{
		if (!effect)
		{
			continue;
		}
		if (!effect->IsActive())
		{
			continue;
		}
		effect->Update(DeltaTime);
	}
}

void CEffectManager::Draw()
{
	for (std::unique_ptr<CEffect>& effect : m_EffectPoolList)
	{
		if (!effect)
		{
			continue;
		}
		if (!effect->IsActive())
		{
			continue;
		}
		effect->Draw();
	}
}

void CEffectManager::Stop(const UINT EffectID)
{
	for (std::unique_ptr<CEffect>& effect : m_EffectPoolList)
	{
		if (!effect)
		{
			continue;
		}
		if (!effect->IsActive())
		{
			continue;
		}
		if (effect->GetID() != EffectID)
		{
			continue;
		}
		effect->Deactivate();
		break;
	}
}

void CEffectManager::UpdatePosition(const UINT EffectID, const Vector3& Position)
{
	for (std::unique_ptr<CEffect>& effect : m_EffectPoolList)
	{
		if (!effect)
		{
			continue;
		}
		if (!effect->IsActive())
		{
			continue;
		}
		if (effect->GetID() != EffectID)
		{
			continue;
		}
		effect->UpdatePosition(Position);
		break;
	}
}
