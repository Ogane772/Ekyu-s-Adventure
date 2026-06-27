#include "Heart.h"
#include "../../../Framework/Model/StaticMesh.h"
#include "../../../Framework/Scene/SceneManager.h"
#include "../../../Framework/Collision/Collision.h"
#include "../../../Framework/Sound/SoundTypes.h"
#include "../../../Framework/Collision/Sphere.h"
#include "../../../Framework/Line/Line.h"
#include "../../../Framework/Shadow/ShadowType.h"
#include "../../Character/Player/Player.h"

namespace
{
	constexpr float RADIUS_SIZE = 1.3f;
	constexpr int HEAL_LIFE = 1;
	constexpr float NO_PLAYER_HIT_TIME = 1.0f;
	constexpr float NO_PLAYER_HIT_INTERVAL = 8.0f / 60.0f;//プレイヤーと当たらない時に点滅を行う際の消滅時間
	constexpr float ROTATION_SPEED = 60.0f;
	constexpr float DROP_SPEED = 2.5f;
}

CHeart::CHeart(const Vector3& SetPos, const bool IsDrop)
{
	m_StateMachine.AddState<CHeart>(
		EHeartState::NORMAL,
		this
	);
	m_StateMachine.AddState<CHeart>(
		EHeartState::DROP,
		this,
		&CHeart::StartDrop,
		&CHeart::UpdateDrop
	);
	m_CullingRadius = RADIUS_SIZE;
	m_Position = SetPos;
	m_Sphere = std::make_unique<CSphere>(SetPos, RADIUS_SIZE);
	UpdateWorldMatrix();
	if (m_Sphere)
	{
		m_Sphere->SetAddSpherePosition(Vector3(0, 1.5f, 0));
		m_Sphere->Update(m_Position);
	}

	if (IsDrop)
	{
		m_StateMachine.SetInitialState(EHeartState::DROP);
	}
	else
	{
		m_StateMachine.SetInitialState(EHeartState::NORMAL);
	}
	m_Scale = Vector3(1.1f, 1.1f, 1.1f);
	m_Model = std::make_unique <CStaticMesh>(EStaticMeshType::HEART);
	if (!m_Model)
	{
		return;
	}
	m_Model->SetEnableLight(true);
	if (MATERIAL* material = m_Model->GetMaterial())
	{
		material->Diffuse = COLOR(1.2f, 1.2f, 1.2f, 1.0f);
		material->Ambient = COLOR(0.42f, 0.42f, 0.42f, 1.0f);
		material->Specular = COLOR(2.0f, 2.0f, 2.0f, 1.0f);
	}
}

void CHeart::UnInit()
{
	MapObjectNull(m_OriginalNumber);
}

void CHeart::Update(float DeltaTime)
{
	if (IsEditMode())
	{
		m_Rotation = Vector3::Zero();
	}
	else
	{
		m_Rotation.y += ROTATION_SPEED * DeltaTime;
	}

	m_StateMachine.Update(DeltaTime);

	UpdateWorldMatrix();
	if (IsEditMode())
	{
		return;
	}

	//プレイヤーとヒットしない際はヒットしない時間カウントを進める
	if (m_IsNoPlayerHit)
	{
		m_FrameTime += DeltaTime;
		if (m_FrameTime > NO_PLAYER_HIT_TIME)
		{
			m_IsNoPlayerHit = false;
		}
		return;
	}

	//プレイヤーとの当たり判定
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
	
	if (CCollison::CollisionCheck(player->GetCapsule(), m_Sphere.get()))
	{
		player->AddHp(HEAL_LIFE);
		PlaySoundIndex(ESoundIndex::HEAL);
		m_IsDestroy = true;
	}
}

void CHeart::Draw()
{
	if (IsInvincible())
	{
		return;
	}
	if (!m_Model)
	{
		return;
	}

	if (!m_IsDisableShadow && !m_IsEditorDisableShadow)
	{
		m_Model->ShadowOnModelDraw(EShadowType::DOWN_SHADOW, m_World);
	}
	else
	{
		m_Model->Draw(m_World);
	}
	if (m_Sphere)
	{
		m_Sphere->Draw();
	}
	if (m_DownLine)
	{
		m_DownLine->Draw();
	}
}

void CHeart::ShadowDraw()
{
	if (!m_Model)
	{
		return;
	}
	if (IsInvincible())
	{
		return;
	}
	if (m_IsDisableShadow || m_IsEditorDisableShadow)
	{
		return;
	}
	m_Model->ShadowDraw(EShadowType::DOWN_SHADOW, m_World);
}

void CHeart::InitBodyCollisionCheck()
{
	if (!m_Sphere)
	{
		return;
	}
	Vector3 norm;
	HitResult hitResult(&m_Position, &norm);
	CCollison::Sphere_VS_Mesh(ECheckCollisionType::OBJECT, m_Sphere.get(), hitResult);
	m_Sphere->Update(m_Position);
}

CSphere* CHeart::GetSphere() const
{
	return m_Sphere.get();
}

MATERIAL* CHeart::GetMaterial() const
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->GetMaterial();
}

void CHeart::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}
}

bool* CHeart::IsEnableLight()
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->IsEnableLight();
}

void CHeart::SetEnableLight(const bool Enable)
{
	if (!m_Model)
	{
		return;
	}
	m_Model->SetEnableLight(Enable);
}

void CHeart::StartDrop()
{
	//画面外でも落下するようにカリングを無効にする
	m_IsNoCullingCheck = true;
	m_IsNoPlayerHit = true;
	m_DownLine = std::make_unique <CLine>(&m_Position, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
}

void CHeart::UpdateDrop(float DeltaTime)
{
	if (!m_DownLine)
	{
		return;
	}
	RayCastInfo downCastInfo;
	//地面との判定
	bool downLineHit = m_DownLine->LineHitCheck(ECheckCollisionType::OBJECT, Vector3(m_Position.x, m_Position.y + 1.5f, m_Position.z), Vector3(m_Position.x, m_Position.y - 0.3f, m_Position.z), &downCastInfo);
	if (downLineHit)
	{
		const float hitPoint = downCastInfo.Point.y;
		if (hitPoint >= m_Position.y)
		{
			m_Position.y = hitPoint;
			m_IsNoCullingCheck = false;
			m_StateMachine.ChangeState(EHeartState::NORMAL);
		}
	}
	else
	{
		m_Position.y -= DROP_SPEED * DeltaTime;
	}

	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}
}

bool CHeart::IsInvincible() const
{
	if (!m_IsNoPlayerHit)
	{
		return false;
	}
	const int invincibleCount = static_cast<int>(m_FrameTime / NO_PLAYER_HIT_INTERVAL) % 2;
	return invincibleCount == 0;
}
