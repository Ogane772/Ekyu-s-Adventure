#include "StarDoor.h"
#include "StageClearDoor.h"
#include "../../../Framework/Object/CommonObject.h"
#include "../../../Framework/Model/StaticMesh.h"
#include "../../../Framework/Scene/Scene.h"
#include "../../../Framework/Scene/SceneManager.h"
#include "../../../Framework/Collision/CollisionTag.h"
#include "../../../Framework/Input/InputInterface.h"
#include "../../../Framework/Camera/Camera.h"
#include "../../../Framework/Sound/SoundTypes.h"
#include "../../../Framework/TextureLoader/TextureTypes.h"
#include "../../../Framework/System/SystemContext.h"
#include "../../Character/Player/Player.h"
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

constexpr float DOWN_DOOR_SPEED = 6.0f;

CStarDoor::CStarDoor(const Vector3& SetPos, const Vector3& SetRot, const int OpenStarCount)
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	m_OpenStarCount = OpenStarCount;
	m_Position = SetPos;
	m_Rotation = SetRot;
	m_Scale = Vector3(1.5f, 1.5f, 1.5f);
	m_StarOpenDoor = new CCommonObject();
	if (m_StarOpenDoor)
	{
		m_StarOpenDoor->Init(EStaticMeshType::STAR_DOOR, m_Position, m_Scale, m_Rotation);
		m_StarOpenDoor->SetPlayerCollisionTag(ECollisionTagName::ALL_COLISION);
		m_StarOpenDoor->SetCullingRadius(10.0f);
		m_StarOpenDoor->SetObjectReference();
		m_StarOpenDoor->SetCameraCollisionTag(ECollisionTagCameraName::BLOCK);
		m_StarOpenDoor = scene.lock()->AddGameObject<CCommonObject>(ESceneObjectType::MESH_OBJECT, m_StarOpenDoor);
	}
	CreateVector();
	UpdateVector();
	m_StageClearDoor = new CStageClearDoor(Vector3(SetPos.x, SetPos.y - 1.0f, SetPos.z), m_Rotation);
	scene.lock()->AddGameObject<CStageClearDoor>(ESceneObjectType::NOMESH_OBJECT, m_StageClearDoor);

	//ドアの開く演出を登録する
	m_DoorAnimationTimeScheduler.Add(0.08f, [=, this]()
	{
		PlayVibration(EVibrationType::EXTRA_LARGE);
		PlaySoundIndex(ESoundIndex::DOOR_OPEN);
	});

	m_DoorAnimationTimeScheduler.Add(2.5f, [=, this]()
	{
		DeleteSoundIndex(ESoundIndex::DOOR_OPEN);
	});

	m_DoorAnimationTimeScheduler.Add(2.83f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::CLEAR_DOOR);
	});

	m_DoorAnimationTimeScheduler.Add(3.16f, [=, this]()
	{
		if (CPlayer* player = GetPlayer())
		{
			player->ChangeFallState();
		}
		if (CSystemContext* system = CSystemContext::GetInstance())
		{
			system->SetEvent(false);
		}
		if (CCamera* camera = GetCamera())
		{
			camera->EndCameraDirection();
		}
		if (m_StarOpenDoor)
		{
			m_StarOpenDoor->SetDestroy();
		}
		ReplayBGM();
		m_IsUpdateDoorAnimation = false;
	});
}

void CStarDoor::Update(float DeltaTime)
{
	if (!m_IsUpdateDoorAnimation)
	{
		return;
	}

	if (IsEndStarDoorAnimation())
	{
		return;
	}

	//ドアを開く演出。時間による効果音やドアを下げる処理を行う

	m_DoorAnimationTimeScheduler.Run(DeltaTime);

	m_Position.y -= DOWN_DOOR_SPEED * DeltaTime;
	if (m_StarOpenDoor)
	{
		Vector3 position = m_StarOpenDoor->GetPosition();
		position.y = m_Position.y;
		m_StarOpenDoor->SetPosition(position);
	}
}

bool CStarDoor::IsEndStarDoorAnimation() const
{
	return m_DoorAnimationTimeScheduler.IsEndTask();
}

void CStarDoor::StartStarDoorAnimation()
{
	if (CCamera* camera = GetCamera())
	{
		const float addCameraLength = -18.0f;
		const Vector3 addPosition = Vector3(m_Front.x * addCameraLength, 5.0f, m_Front.z * addCameraLength);
		const Vector3 cameraFront = Vector3(m_Front.x, -0.14f, m_Front.z);
		const float cameraLength = 12.0f;
		camera->StartCameraDirection(m_Position + addPosition, cameraFront, cameraLength);
	}
	m_IsUpdateDoorAnimation = true;
	m_DoorAnimationTimeScheduler.Reset();
	if (m_StageClearDoor)
	{
		m_StageClearDoor->EnableDraw(true);
	}
	if (CSystemContext* system = CSystemContext::GetInstance())
	{
		system->SetEvent(true);
	}
}

bool CStarDoor::IsStartStarDoorAnimation(const int GetStarCount) const
{
	return GetStarCount == m_OpenStarCount;
}

void CStarDoor::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_StarOpenDoor)
	{
		m_StarOpenDoor->SetPosition(m_Position);
	}
	if (m_StageClearDoor)
	{
		m_StageClearDoor->SetPosition(SetPos);
	}
}

void CStarDoor::SetRotation(const Vector3& SetRot)
{
	m_Rotation = SetRot;
	if (m_StarOpenDoor)
	{
		m_StarOpenDoor->SetRotation(m_Rotation);
	}
}

void CStarDoor::ChangeTexture(const ETextureType Type, const int Index)
{
	if (m_StarOpenDoor)
	{
		m_StarOpenDoor->ChangeTexture(Type, Index);
	}
}

int* CStarDoor::GetOpenStar()
{
	return &m_OpenStarCount;
}

void CStarDoor::SetDestroyEdit()
{
	m_IsDestroy = true;
	if (m_StarOpenDoor)
	{
		m_StarOpenDoor->SetDestroy();
	}
	if (m_StageClearDoor)
	{
		m_StageClearDoor->SetDestroy();
	}
}

void CStarDoor::SetStarTexture()
{
	if (!m_StarOpenDoor)
	{
		return;
	}

	switch (m_OpenStarCount)
	{
	case 1:
		m_StarOpenDoor->ChangeTexture(ETextureType::STAR_DOOR1, 0);
		break;
	case 2:
		m_StarOpenDoor->ChangeTexture(ETextureType::STAR_DOOR2, 0);
		break;
	case 3:
		m_StarOpenDoor->ChangeTexture(ETextureType::STAR_DOOR3, 0);
		break;
	case 4:
		m_StarOpenDoor->ChangeTexture(ETextureType::STAR_DOOR4, 0);
		break;
	case 5:
		m_StarOpenDoor->ChangeTexture(ETextureType::STAR_DOOR5, 0);
		break;
	case 6:
		m_StarOpenDoor->ChangeTexture(ETextureType::STAR_DOOR6, 0);
		break;
	case 7:
		m_StarOpenDoor->ChangeTexture(ETextureType::STAR_DOOR7, 0);
		break;
	}
}