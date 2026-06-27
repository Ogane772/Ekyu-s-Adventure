#include "PlayerStart.h"
#include "../../../Framework/Model/StaticMesh.h"
#include "../../../Framework/Collision/Sphere.h"

CPlayerStart::CPlayerStart(const Vector3& SetPos, const Vector3& SetRot)
{
	m_Position = SetPos;
	m_Rotation = SetRot;
	m_Sphere = std::make_unique<CSphere>(SetPos, 0.3f);
	m_Scale = Vector3(1.0f, 1.0f, 1.0f);
	m_Model = std::make_unique<CStaticMesh>(EStaticMeshType::PLAYER_START);
}

void CPlayerStart::Update(float DeltaTime)
{
	if (!IsEditMode())
	{
		return;
	}
	UpdateWorldMatrix();
}

void CPlayerStart::Draw()
{
	if (!IsEditMode())
	{
		return;
	}
	if (m_Model)
	{
		m_Model->Draw(m_World);
	}
}

void CPlayerStart::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}
}