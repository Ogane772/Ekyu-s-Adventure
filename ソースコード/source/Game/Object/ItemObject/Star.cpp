#include "Star.h"
#include "../../../Framework/Model/StaticMesh.h"
#include "../../../Framework/Scene/SceneManager.h"
#include "../../../Framework/Collision/Collision.h"
#include "../../../Framework/Camera/Camera.h"
#include "../../../Framework/Collision/Sphere.h"
#include "../../../Framework/Shadow/ShadowType.h"
#include "../../../Framework/Sound/SoundTypes.h"
#include "../../Character/Player/Player.h"

namespace
{
	constexpr float RADIUS_SIZE = 1.2f;
	constexpr float WAIT_EMERGENCE_TIME = 1.0f;//スター登場時の跳ねる挙動終了後の待機時間
	constexpr float NORMAL_ROTATION_SPEED = 250.0f;
	constexpr float EMERGENCE_ROTATION_SPEED = 100.0f;
}

CStar::CStar(const Vector3 SetPos)
{
	m_StateMachine.AddState<CStar>(
		EStarState::NORMAL,
		this,
		nullptr,
		&CStar::UpdateNormal
	);
	m_StateMachine.AddState<CStar>(
		EStarState::EMERGENCE,
		this,
		&CStar::StartEmergence,
		&CStar::UpdateEmergence
	);
	m_EmergenceParameterList =
	{
		EmergenceParameter(0.8f, 9.0f),
		EmergenceParameter(0.7f, 8.0f),
		EmergenceParameter(0.6f, 7.0f),
	};
	m_StateMachine.SetInitialState(EStarState::NORMAL);
	m_CullingRadius = RADIUS_SIZE;
	m_Sphere = std::make_unique<CSphere>(SetPos, RADIUS_SIZE);
	if (m_Sphere)
	{
		m_Sphere->SetAddSpherePosition(Vector3(0, 1.3f, 0));
	}
	m_Position = SetPos;
	m_Rotation = Vector3::Zero();
	m_Scale = Vector3(1.0f,1.0f,1.0f);
	UpdateWorldMatrix();
	m_Model = std::make_unique<CStaticMesh>(EStaticMeshType::STAR);
	if (!m_Model)
	{
		return;
	}
	m_Model->SetEnableLight(true);
	if (MATERIAL* material = m_Model->GetMaterial())
	{
		material->Diffuse = COLOR(2.0f, 2.0f, 2.0f, 1.0f);
		material->Ambient = COLOR(1.65f, 1.65f, 1.65f, 1.0f);
		material->Specular = COLOR(2.0f, 2.0f, 2.0f, 1.0f);
	}
}

void CStar::UnInit()
{
	MapObjectNull(m_OriginalNumber);
}

void CStar::Update(float DeltaTime)
{
	m_StateMachine.Update(DeltaTime);

	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}

	UpdateWorldMatrix();
	//プレイヤーとの当たり判定
	if (IsEditMode())
	{
		return;
	}
	if (m_StateMachine.GetState() == EStarState::EMERGENCE)
	{
		return;
	}
	CPlayer* player = GetPlayer();
	if (!player)
	{
		return;
	}
	if (player->IsDamageState() || player->IsDeath())
	{
		return;
	}
	
	if (CCollison::CollisionCheck(player->GetCapsule(), m_Sphere.get()))
	{
		player->AddMyStar();
		player->ChangeCongratulationState();
		if (m_IsStarSceneChange)
		{
			player->SetEnableGetStageClearStar();
		}
		m_IsDestroy = true;
	}
}

void CStar::Draw()
{
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
}

void CStar::ShadowDraw()
{
	if (m_IsDisableShadow || m_IsEditorDisableShadow)
	{
		return;
	}
	if (m_StateMachine.GetState() == EStarState::EMERGENCE)
	{
		return;
	}
	if (m_Model)
	{
		m_Model->ShadowDraw(EShadowType::DOWN_SHADOW, m_World);
	}
}

void CStar::SetEmergenceState()
{
	m_StateMachine.ChangeState(EStarState::EMERGENCE);
}

void CStar::InitBodyCollisionCheck()
{
	Vector3 norm;
	HitResult hitResult(&m_Position, &norm);
	CCollison::Sphere_VS_Mesh(ECheckCollisionType::OBJECT, m_Sphere.get(), hitResult);
	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}
}

CSphere* CStar::GetSphere() const
{
	return m_Sphere.get();
}

MATERIAL* CStar::GetMaterial() const
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->GetMaterial();
}

void CStar::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_Sphere)
	{
		m_Sphere->Update(m_Position);
	}
}

bool* CStar::IsEnableLight()
{
	if (!m_Model)
	{
		return nullptr;
	}
	return m_Model->IsEnableLight();
}

void CStar::SetEnableLight(const bool Enable)
{
	if (!m_Model)
	{
		return;
	}
	m_Model->SetEnableLight(Enable);
}

void CStar::UpdateNormal(float DeltaTime)
{
	if (IsEditMode())
	{
		m_Rotation = Vector3::Zero();
	}
	else
	{
		m_Rotation.y += NORMAL_ROTATION_SPEED * DeltaTime;;
	}
}

void CStar::StartEmergence()
{
	SetEvent(true);
	m_DefaultBounceHeight = m_Position.y;
	m_BounceCount = 0;
	m_FrameTime = 0.0f;
	PlaySoundIndex(ESoundIndex::STAR_MOVE);
}

void CStar::UpdateEmergence(float DeltaTime)
{
	CCamera* camera = GetCamera();
	if (!camera)
	{
		return;
	}
	camera->SetCameraCenterPosition(m_Position);
	m_Rotation.y += EMERGENCE_ROTATION_SPEED * DeltaTime;

	const bool isEndBounce = m_BounceCount >= static_cast<int>(m_EmergenceParameterList.size());
	//バウンスが終了していれば一定時間経過後、通常ステートにする
	if (isEndBounce)
	{
		if (m_FrameTime >= WAIT_EMERGENCE_TIME)
		{
			SetEvent(false);
			m_StateMachine.ChangeState(EStarState::NORMAL);
		}
	}
	else
	{
		//m_EmergenceParameterListに登録された分だけバウンスを行う。
		const EmergenceParameter& parameter = m_EmergenceParameterList[m_BounceCount];
		const float addHeight = fabsf(sinf(m_FrameTime * XM_PI / parameter.BounceTime)) * parameter.BounceHeight;
		m_Position.y = m_DefaultBounceHeight + addHeight;

		//現在のバウンスが終了したらカウントを進める
		if (m_FrameTime >= parameter.BounceTime)
		{
			PlaySoundIndex(ESoundIndex::STAR_MOVE);
			m_BounceCount++;
			m_FrameTime = 0.0f;
		}
	}
	m_FrameTime += DeltaTime;
}