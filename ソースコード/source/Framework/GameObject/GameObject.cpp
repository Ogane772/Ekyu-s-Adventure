#include "../GameObject/GameObject.h"
#include "../Camera/Camera.h"
#include "../Scene/SceneManager.h"
#include "../Object/CommonObject.h"
#include "../Collision/Box.h"
#include "../Input/InputInterface.h"
#include "../Mouse/MouseCursor.h"
#include "../Sound/SoundManager.h"
#include "../../Game/Effect/EffectManager.h"
#include "../System/SystemContext.h"

void CGameObject::PreUpdate(float DeltaTime)
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}

	if (system->IsPause())
	{
		return;
	}

	if (m_IsNoCullingCheck)
	{
		m_IsCulling = true;
	}
	else
	{
		Vector3 cullingPosition = m_Position;
		if (m_IsOverrideCullingPosition)
		{
			cullingPosition = m_OverrideCullingPosition;
		}
		m_IsCulling = system->VFCulling(cullingPosition, m_CullingRadius);
	}
	Update(DeltaTime);
}

void CGameObject::PreDraw()
{
	if (!m_IsCulling)
	{
		return;
	}
	Draw();
}

void CGameObject::PreShadowDraw()
{
	if (!m_IsCulling)
	{
		return;
	}
	ShadowDraw();
}

void CGameObject::CreateVector()
{
	//ベクトル生成
	m_Front = Vector3::Forward();
	m_Right = Vector3::Right();
	m_Up = Vector3::Up();
	//正規化
	m_Front.Normalize();
	m_Right = Vector3::Cross(m_Front, m_Up);
	m_Right.Normalize();
	m_Up = Vector3::Cross(m_Right, m_Front);
	m_Up.Normalize();
}

void CGameObject::TargetRotation(const float RotationSpeed, float DeltaTime, const Vector3 TargetRotation)
{
	const float speed = (std::min)(RotationSpeed * DeltaTime, 1.0f);
	const XMVECTOR quaternionStartRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
	const XMVECTOR quaternionGoalRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(TargetRotation.x), XMConvertToRadians(TargetRotation.y), XMConvertToRadians(TargetRotation.z));
	const XMVECTOR quaternionCurrentRotation = XMQuaternionSlerp(quaternionStartRotation, quaternionGoalRotation, speed);

	const XMMATRIX rotMatrix = XMMatrixRotationQuaternion(quaternionCurrentRotation);

	//Matrixの値をオイラー角としてVector3に変換
	const float pitch = asinf(-rotMatrix.r[2].m128_f32[1]);
	const float yaw = atan2f(rotMatrix.r[2].m128_f32[0], rotMatrix.r[2].m128_f32[2]);
	const float roll = atan2f(rotMatrix.r[0].m128_f32[1], rotMatrix.r[1].m128_f32[1]);
	m_Rotation = Vector3(XMConvertToDegrees(pitch), XMConvertToDegrees(yaw), XMConvertToDegrees(roll));
}

void CGameObject::UpdateVector()
{
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
	m_Front = Vector3::TransformCoord(Vector3::Forward(), rotationMatrix);
	m_Right = Vector3::TransformCoord(Vector3::Right(), rotationMatrix);
	m_Up = Vector3::TransformCoord(Vector3::Up(), rotationMatrix);
}

void CGameObject::MapObjectNull(const int Number)
{
	if (Number == -1)
	{
		return;
	}
	CSceneManager* manager = CSceneManager::GetInstance();
	if (!manager)
	{
		return;
	}
	CMouseCursor* mouse = manager->GetMouseCursor();
	if (mouse)
	{		
		mouse->SetMapObjectNull(Number);
	}
}

void CGameObject::UpdateWorldMatrix()
{
	m_World = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	m_World *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
	m_World *= XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
}

void CGameObject::PlayVibration(const EVibrationType Type)
{
	if (CInputInterface* instance = CInputInterface::GetInstance())
	{
		instance->PlayVibration(Type);
	}
}

UINT CGameObject::PlayEffect(const EEffectType Type, const Vector3& Position, const float Size)
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return 0;
	}
	CEffectManager* manager = sceneManager->GetEffectManager();
	if (!manager)
	{
		return 0;
	}
	return manager->Active(Type, Position, Size);
}

void CGameObject::StopEffect(const UINT EffectID)
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	CEffectManager* manager = sceneManager->GetEffectManager();
	if (!manager)
	{
		return;
	}
	manager->Stop(EffectID);
}

void CGameObject::UpdateEffectPosition(const UINT EffectID, const Vector3& Position)
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	CEffectManager* manager = sceneManager->GetEffectManager();
	if (!manager)
	{
		return;
	}
	manager->UpdatePosition(EffectID, Position);
}

UINT CGameObject::Play3DSoundIndex(const ESoundIndex SoundIndex, const Vector3& Position)
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		return instance->Play3DSound(SoundIndex, Position);
	}
	return UINT_MAX;
}

void CGameObject::Update3DSoundPosition(const UINT Id, const Vector3& Position)
{
	CSoundManager* instance = CSoundManager::GetInstance();
	if (instance)
	{
		instance->Update3DSoundPosition(Id, Position);
	}
}