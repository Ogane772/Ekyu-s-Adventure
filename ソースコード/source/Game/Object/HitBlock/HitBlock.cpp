#include "HitBlock.h"
#include "../../../Framework/Object/CommonObject.h"
#include "../../../Framework/Scene/Scene.h"
#include "../../../Framework/Scene/SceneManager.h"
#include "../../../Framework/Collision/Box.h"
#include "../../../Framework/Collision/CollisionTag.h"

namespace
{
	constexpr float ROTATION_SPPED = 2.0f;
	constexpr float ADD_HEIGHT = 12.0f;
	constexpr float MOVE_TIME = 1.0f;
}

CHitBlock::CHitBlock(const Vector3& SetPos, const Vector3& SetRot, const Vector3& SetScale, const Vector3& SetAddPos, const Vector3& Max, const Vector3& Min)
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	m_Position = SetPos;
	m_Rotation = SetRot;
	m_Scale = SetScale;
	m_TargetRotation = m_Rotation;
	if (m_Common = new CCommonObject())
	{
		m_Common->Init(EStaticMeshType::THORN_BOX, m_Position, m_Scale, m_Rotation);
		m_Common->SetPlayerCollisionTag(ECollisionTagName::NO_CLIFF_HIT);
		m_Common->SetCameraCollisionTag(ECollisionTagCameraName::CLEAR);
		m_Common->SetCullingRadius(30.0f);
		m_Common->SetDisableShadow(true);
		m_Common = scene.lock()->AddGameObject<CCommonObject>(ESceneObjectType::MESH_OBJECT, m_Common);
		m_Common->SetStepsCollisionTag(ECollisionTagSteps::ROCK);
	}
	if (MATERIAL* material = m_Common->GetMaterial())
	{
		material->Diffuse = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
		material->Ambient = COLOR(0.0f, 0.0f, 0.0f, 1.0f);
		material->Specular = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}
	m_Box = std::make_unique<CBox>(m_Position, Max, Min);
	if (m_Box)
	{
		m_Box->SetPosition(m_Position);
		m_Box->AddBoxPosition(SetAddPos);
		m_Box->Update(m_Position, m_Rotation);
	}
}

void CHitBlock::Update(float DeltaTime)
{
	if (!m_Hit)
	{
		return;
	}

	if (m_FrameTime < MOVE_TIME)
	{
		m_Position.y -= ADD_HEIGHT * DeltaTime;
		m_FrameTime += DeltaTime;
	}
	else
	{
		m_IsDestroy = true;
	}
	TargetRotation(ROTATION_SPPED, DeltaTime, Vector3(m_TargetRotation.x, m_TargetRotation.y, m_TargetRotation.z));
	if (m_Common)
	{
		m_Common->SetRotation(m_Rotation);
		m_Common->SetPosition(m_Position);
	}
}

void CHitBlock::SetHit(const Vector3& HitObjectFront)
{
	m_Hit = true;
	//“–‚½‚è”»’è‚ð–³Œø‰»
	if (m_Box)
	{
		m_Box->SetBoxSize(Vector3(0, 0, 0), Vector3(0, 0, 0));
	}
	const Vector3 backVector = m_Position + HitObjectFront;
	//“|‚ê‚éŒ©‚½–Ú‚ð•\Œ»‚·‚é‚½‚ß‰ñ“]’l‚ðŒvŽZ
	m_TargetRotation.z = XMConvertToDegrees(atan2(m_Position.x - backVector.x, 0.0f));
	m_TargetRotation.x = -XMConvertToDegrees(atan2(m_Position.z - backVector.z, 0.0f));
}