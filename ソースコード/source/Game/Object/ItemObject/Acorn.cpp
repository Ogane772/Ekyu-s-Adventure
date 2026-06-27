#include "Acorn.h"
#include "../../Effect/EffectTypes.h"
#include "../../../Framework/Scene/Scene.h"
#include "../../../Framework/Scene/SceneManager.h"
#include "../../../Framework/Collision/Sphere.h"
#include "../../../Framework/Collision/Collision.h"
#include "../../../Framework/Sound/SoundTypes.h"
#include "../../../Framework/TextureLoader/TextureTypes.h"
#include "../../Character/Player/Player.h"

namespace
{
	constexpr float RADIUS_SIZE = 1.0f;
	constexpr float INIT_ADD_HEIGHT = 1.0f;
}

CAcorn::CAcorn(const Vector3 SetPos)
{
	m_CullingRadius = RADIUS_SIZE;
	m_Texture = LoadTexture(ETextureType::ACORN_ANIME);
	m_Sphere = std::make_unique<CSphere>(SetPos, RADIUS_SIZE);
	m_Position = SetPos;
	m_EffectId = PlayEffect(EEffectType::ACORN, Vector3(m_Position), 2.0f);
}

void CAcorn::UnInit()
{
	ReleaseTexture(m_Texture);
	MapObjectNull(m_OriginalNumber);
}

void CAcorn::Update(float DeltaTime)
{
	if (!m_IsCulling)
	{
		return;
	}
	if (IsEditMode())
	{
		return;
	}
;
	//ƒvƒŒƒCƒ„[‚Æ‚Ì“–‚½‚è”»’è
	CPlayer* player = GetPlayer();
	if (!player)
	{
		return;
	}
	if (*player->IsDebugNoItemHit())
	{
		return;
	}
	if (player->IsDeath())
	{
		return;
	}

	if(CCollison::CollisionCheck(player->GetCapsule(), m_Sphere.get()))
	{
		m_IsDestroy = true;
		player->AddMyAcorn();
		StopEffect(m_EffectId);
		PlayEffect(EEffectType::ITEM_GET, m_Position, 3.0f);
		PlaySoundIndex(ESoundIndex::ACORN_HIT);
	}
}

void CAcorn::Draw()
{
	if (m_Sphere)
	{
		m_Sphere->Draw();
	}
}

void CAcorn::InitBodyCollisionCheck()
{
	m_Position.y += INIT_ADD_HEIGHT;
	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}
	Vector3 norm;
	HitResult hitResult(&m_Position, &norm);
	CCollison::Sphere_VS_Mesh(ECheckCollisionType::OBJECT, m_Sphere.get(), hitResult);
	UpdateEffectPosition(m_EffectId, m_Position);
	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}
}

CSphere* CAcorn::GetSphere() const
{
	return m_Sphere.get();
}

void CAcorn::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	UpdateEffectPosition(m_EffectId, m_Position);
	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}
}

void CAcorn::SetDestroyEdit()
{
	StopEffect(m_EffectId);
	m_IsDestroy = true;
}
