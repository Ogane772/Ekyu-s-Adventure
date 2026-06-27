#include "StageClearDoor.h"
#include "../../../Framework/Model/StaticMesh.h"
#include "../../../Framework/Collision/Box.h"
#include "../../../Framework/Scene/Scene.h"
#include "../../../Framework/Scene/SceneManager.h"
#include "../../../Framework/Collision/Collision.h"
#include "../../../Framework/Shadow/ShadowType.h"
#include "../../Character/Player/Player.h"

namespace
{
	constexpr float CULLING_RADIUS = 3.0f;
};

CStageClearDoor::CStageClearDoor(const Vector3& SetPos, const Vector3& SetRot)
{
	m_CullingRadius = CULLING_RADIUS;
	m_Position = SetPos;
	m_Rotation = SetRot;
	m_Scale = Vector3(4.5f, 4.5f, 4.5f);
	m_Model = std::make_unique <CStaticMesh>(EStaticMeshType::CLEAR_DOOR);
	m_BoxCollision = std::make_unique<CBox>(m_Position, Vector3(7.5f, 3.5f, 7.5f), Vector3(0.0f, 0.0f, 0.0f));
	if (m_BoxCollision)
	{
		m_BoxCollision->AddBoxPosition(Vector3(0.0f, 3.5f, 0.0f));
	}
}

void CStageClearDoor::Update(float DeltaTime)
{
	if (!m_EnableDraw)
	{
		return;
	}
	if (!m_IsCulling)
	{
		return;
	}

	if (m_BoxCollision)
	{
		m_BoxCollision->Update(m_Position, m_Rotation);
	}

	UpdateWorldMatrix();

	CPlayer* player = GetPlayer();
	if (!player)
	{
		return;
	}

	if (player->IsDamageState() || player->IsDeath())
	{
		player->SetEnableClearButton(false);
		return;
	}
	if (CCollison::CollisionCheck(player->GetCapsule(), m_BoxCollision.get()))
	{
		player->SetEnableClearButton(true);
		return;
	}
	player->SetEnableClearButton(false);
}

void CStageClearDoor::Draw()
{
	if (!m_EnableDraw)
	{
		return;
	}
	if (!m_IsCulling)
	{
		return;
	}

	if (m_Model)
	{
		m_Model->ShadowOnModelDraw(EShadowType::NO_DOWN_SHADOW, m_World);
	}

	if (m_BoxCollision)
	{
		m_BoxCollision->Draw();
	}
}

void CStageClearDoor::ShadowDraw()
{
	if (!m_EnableDraw)
	{
		return;
	}
	if (!m_IsCulling)
	{
		return;
	}

	if (m_Model)
	{
		m_Model->ShadowDraw(EShadowType::NO_DOWN_SHADOW, m_World);
	}
}
