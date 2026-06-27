#include "Arrow.h"
#include "../../../Framework/Model/StaticMesh.h"

namespace
{
	constexpr float ROTATION_SPEED = 300.0f;
}

CArrow::CArrow(const Vector3& SetPos)
{
	m_Position = SetPos;
	m_Scale = Vector3(2.1f, 2.1f, 2.1f);
	m_Model = std::make_unique<CStaticMesh>(EStaticMeshType::ARROW);
}

void CArrow::Update(float DeltaTime)
{
	m_Rotation.y += ROTATION_SPEED * DeltaTime;
	UpdateWorldMatrix();
}

void CArrow::Draw()
{
	if (m_Model)
	{
		m_Model->Draw(m_World);
	}
}
