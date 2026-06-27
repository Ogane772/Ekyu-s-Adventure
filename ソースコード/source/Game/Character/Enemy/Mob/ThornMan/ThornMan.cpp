#include "ThornMan.h"
#include "../../../Player/Player.h"
#include "../../../../Effect/EffectTypes.h"
#include "../../../../../Framework/Collision/Sphere.h"
#include "../../../../../Framework/Collision/Box.h"
#include "../../../../../Framework/Collision/Capsule.h"
#include "../../../../../Framework/Collision/Collision.h"
#include "../../../../../Framework/Scene/SceneManager.h"
#include "../../../../../Framework/Scene/Scene.h"
#include "../../../../../Framework/Object/CommonObject.h"
#include "../../../../../Framework/Line/Line.h"
#include "../../../../../Framework/Sound/SoundTypes.h"
#include "../../../../../Framework/Input/InputInterface.h"
#include "../../../../../Framework/HitStop/HitStop.h"
#include "../../../../../Framework/Shadow/ShadowType.h"

namespace
{
	constexpr int LIFE = 1;
	constexpr int PLAYER_DAMAGE = 1;
	constexpr float CULLING_RADIUS = 3.0f;
	constexpr float WAIT_ROTATION_SPEED = 5.0f;
	constexpr float DISCOVERY_ROTATION_SPEED = 8.0f;
	constexpr float WAIT_TIME = 2.0f;
	constexpr float ADD_HIT_EFFECT_HEIGHT = 2.5f;
	constexpr float HIT_EFFECT_SIZE = 5.5f;
	constexpr float ADD_DEATH_EFFECT_HEIGHT = 1.5f;
	constexpr float DEATH_EFFECT_SIZE = 5.5f;
	constexpr float ATTACK_TIME = 1.5f;
	constexpr float RANGE_CAPSULE_HEIGHT = 1.8f;
	constexpr float RANGE_CAPSULE_LENGTH = 10.0f;
};

CThornMan::CThornMan(Vector3 pos)
{
	m_AnimationDataList =
	{
		{ EThornManAnimationNumber::IDLE,			AnimationData(0.5f, true)  },
		{ EThornManAnimationNumber::WALK,			AnimationData(0.3f, true)  },
		{ EThornManAnimationNumber::PLAYER_MOVE,	AnimationData(0.5f, true)  },
		{ EThornManAnimationNumber::DISCOVERY,		AnimationData(0.5f, false) },
		{ EThornManAnimationNumber::DEATH,			AnimationData(0.5f, false) },
	};

	m_SpeedParameterList =
	{
		{ EThornManSpeedParameterType::WALK, SpeedParameter(0.0f, 0.08f, 10.0f, 10.0f, 8.0f) },
		{ EThornManSpeedParameterType::PLAYER_MOVE, SpeedParameter(0.0f, 0.3f, 8.0f, 1.0f, 10.0f) },
	};

	m_StateMachine.AddState<CThornMan>(
		EThornManState::IDLE,
		this,
		&CThornMan::StartIdle,
		&CThornMan::UpdateIdle,
		nullptr
	);
	m_StateMachine.AddState<CThornMan>(
		EThornManState::WALK,
		this,
		&CThornMan::StartWalk,
		&CThornMan::UpdateWalk,
		nullptr
	);
	m_StateMachine.AddState<CThornMan>(
		EThornManState::PLAYER_MOVE,
		this,
		&CThornMan::StartPlayerMove,
		&CThornMan::UpdatePlayerMove,
		nullptr
	);
	m_StateMachine.AddState<CThornMan>(
		EThornManState::DISCOVERY,
		this,
		&CThornMan::StartDiscovery,
		&CThornMan::UpdateDiscovery,
		nullptr
	);
	m_StateMachine.AddState<CThornMan>(
		EThornManState::DEATH,
		this,
		&CThornMan::StartDeath,
		&CThornMan::UpdateDeath,
		nullptr
	);
	m_StateMachine.AddState<CThornMan>(
		EThornManState::RESPAWN_WAIT,
		this,
		nullptr,
		&CThornMan::UpdateRespawnWait,
		nullptr
	);

	m_CullingRadius = CULLING_RADIUS;
	m_EnemyType = EEnemyType::THORN_MAN;
	m_Position = pos;
	m_FirstPosition = m_Position;
	m_PrevPosition = m_Position;
	m_Scale = Vector3(16, 16, 16);
	m_Rotation = Vector3(0, 180, 0);
	m_FirstRotation = m_Rotation;
	m_FirstScale = m_Scale;
	m_HP.SetMaxHP(LIFE);
	m_HP.SetHP(LIFE);
	CreateVector();
	m_SkeletalMesh = std::make_unique<CSkeletalMesh>(ESkeletalMeshType::THORN_MAN);

	//RangeSphereÇÕíTímîÕàÕÇ»ÇÃÇ≈èâä˙ílÇ©ÇÁìÆÇ©Ç≥Ç»Ç¢
	m_RangeCapsule = std::make_unique<CCapsule>(Vector3(0, RANGE_CAPSULE_HEIGHT, 0), Vector3(m_Front.x * RANGE_CAPSULE_LENGTH, RANGE_CAPSULE_HEIGHT, m_Front.z * RANGE_CAPSULE_LENGTH), Vector3(0.1f, 0.1f, 0.1f), 0.95f, 8, 8);
	if (m_RangeCapsule)
	{
		m_RangeCapsule->Update(m_Position);
	}
	m_BodySphere = std::make_unique<CSphere>(m_FirstPosition, 1.6f);
	if (m_BodySphere)
	{
		m_BodySphere->SetAddSpherePosition(Vector3(0, 1.7f, 0));
		m_BodySphere->Update(m_Position);
	}
	m_StateMachine.SetInitialState(EThornManState::IDLE);
}

void CThornMan::Update(float DeltaTime)
{
	if (IsEvent())
	{
		return;
	}
	if (IsEditMode())
	{
		m_IsCulling = true;
		m_FirstRotation = m_Rotation;
		m_FirstScale = m_Scale;
		m_NowTargetPoint = 0;
		if (m_BodySphere)
		{
			m_BodySphere->Update(m_Position);
		}
		UpdateWorldMatrix();
		return;
	}
	if (m_StateMachine.GetState() == EThornManState::RESPAWN_WAIT)
	{
		m_StateMachine.Update(DeltaTime);
		return;
	}
	m_PrevPosition = m_Position;

	if (m_SkeletalMesh)
	{
		m_SkeletalMesh->Update(DeltaTime);
	}

	m_StateMachine.Update(DeltaTime);

	BodyCollisionCheck(DeltaTime);

	if (m_RangeCapsule)
	{
		m_RangeCapsule->SetCapsulePosition(Vector3(0, RANGE_CAPSULE_HEIGHT, 0), Vector3(m_Front.x * RANGE_CAPSULE_LENGTH, RANGE_CAPSULE_HEIGHT, m_Front.z * RANGE_CAPSULE_LENGTH));
		m_RangeCapsule->Update(m_Position);
	}
	if (m_BodySphere)
	{
		m_BodySphere->Update(m_Position);
	}

	UpdateWorldMatrix();
}

void CThornMan::Draw()
{
	if (!m_SkeletalMesh)
	{
		return;
	}
	if (IsRespawnWait())
	{
		return;
	}
	if (!m_IsCulling)
	{
		return;
	}

	if (m_IsEditorDisableShadow)
	{
		m_SkeletalMesh->Draw(m_World);
	}
	else
	{
		m_SkeletalMesh->ShadowOnModelDraw(EShadowType::DOWN_SHADOW, m_World);
	}
	if (m_RangeCapsule)
	{
		m_RangeCapsule->Draw();
	}
	if (m_BodySphere)
	{
		m_BodySphere->Draw();
	}
}

void CThornMan::SingleCollisonDraw()
{
	if (m_RangeCapsule)
	{
		m_RangeCapsule->SingleDraw();
	}
	if (m_BodySphere)
	{
		m_BodySphere->SingleDraw();
	}
}

void CThornMan::PlayerHitCheck()
{
	PlayerHitCheckEntity(LIFE, LIFE, 0.0f, true);
}

void CThornMan::Damage(int Damage)
{
	PlayVibration(EVibrationType::LOW);
	PlayHitStop(EHitStopType::LOW);
	StopSoundID(m_WalkSE);
	StopSoundID(m_DashSE);
	CreateHitEffect(Vector3(m_Position.x, m_Position.y + ADD_HIT_EFFECT_HEIGHT, m_Position.z), HIT_EFFECT_SIZE);
	m_HP.AddDamage(Damage);
	m_StateMachine.ChangeState(EThornManState::DEATH);
}

void CThornMan::BodyCollisionCheck(const float DeltaTime)
{
	if (IsRespawnWait())
	{
		return;
	}

	UpdateEnemyCollision();
	UpdateMeshVsBodyCollision();
	UpdateGroundCollision(DeltaTime);
}

void CThornMan::SetBodySphere(const Vector3 local_pos, const float radius)
{
	if (m_BodySphere)
	{
		m_BodySphere->SetAddSpherePosition(local_pos);
		m_BodySphere->SetRadius(radius);
		m_BodySphere->Update(m_FirstPosition);
	}
}

bool CThornMan::IsRespawnWait() const
{
	return m_StateMachine.GetState() == EThornManState::RESPAWN_WAIT;
}

void CThornMan::StartIdle()
{
	PlayAnimation<EThornManAnimationNumber>(m_AnimationDataList, EThornManAnimationNumber::IDLE, true);
	StopSoundID(m_WalkSE);
	StopSoundID(m_DashSE);
}

void CThornMan::UpdateIdle(float DeltaTime)
{
	if (IsPlayerRange())
	{
		m_StateMachine.ChangeState(EThornManState::DISCOVERY);
		return;
	}
	Vector3 direction = m_TargetPoint[m_NowTargetPoint] - m_Position;
	direction.y = 0.0f;

	DirectionRotation(WAIT_ROTATION_SPEED, DeltaTime, direction);
	UpdateVector();
	if (m_StateMachine.GetCurrentStateTime() > WAIT_TIME)
	{
		m_StateMachine.ChangeState(EThornManState::WALK);
	}
}

void CThornMan::StartWalk()
{
	PlayAnimation<EThornManAnimationNumber>(m_AnimationDataList, EThornManAnimationNumber::WALK, true);
	if (m_WalkSE == UINT_MAX)
	{
		m_WalkSE = Play3DSoundIndex(ESoundIndex::SLIME_WALK, m_Position);
	}
	else
	{
		PlaySoundID(m_WalkSE);
	}
	StopSoundID(m_DashSE);
}

void CThornMan::UpdateWalk(float DeltaTime)
{
	if (!m_Player)
	{
		return;
	}
	if (CCollison::CollisionCheck(m_Player->GetCapsule(), m_RangeCapsule.get()))
	{
		m_StateMachine.ChangeState(EThornManState::DISCOVERY);
		return;
	}
	if (m_MaxTargetPoint == -1 || !m_SpeedParameterList.contains(EThornManSpeedParameterType::WALK))
	{
		return;
	}

	Vector3 direction = m_TargetPoint[m_NowTargetPoint] - m_Position;
	direction.y = 0.0f;

	SpeedParameter& speedParam = m_SpeedParameterList[EThornManSpeedParameterType::WALK];
	speedParam.MoveDirection = direction.Normalized();
	Move(speedParam, DeltaTime);
	if (m_WalkSE != UINT_MAX)
	{
		Update3DSoundPosition(m_WalkSE, m_Position);
	}
	if (IsTargetGoal(direction))
	{
		m_NowTargetPoint++;
		if (m_MaxTargetPoint < m_NowTargetPoint)
		{
			m_NowTargetPoint = 0;
		}
		m_StateMachine.ChangeState(EThornManState::IDLE);
	}
}

void CThornMan::StartPlayerMove()
{
	PlayAnimation<EThornManAnimationNumber>(m_AnimationDataList, EThornManAnimationNumber::WALK, true);
	if (m_DashSE == UINT_MAX)
	{
		m_DashSE = Play3DSoundIndex(ESoundIndex::SLIME_DASH, m_Position);
	}
	else
	{
		PlaySoundID(m_DashSE);
	}
	StopSoundID(m_WalkSE);
}

void CThornMan::UpdatePlayerMove(float DeltaTime)
{
	if (!m_Player)
	{
		return;
	}
	SpeedParameter& speedParam = m_SpeedParameterList[EThornManSpeedParameterType::PLAYER_MOVE];
	speedParam.MoveDirection = m_Front;
	Move(speedParam, DeltaTime);
	if (m_StateMachine.GetCurrentStateTime() > ATTACK_TIME)
	{
		m_StateMachine.ChangeState(EThornManState::IDLE);
	}
	if (m_DashSE != UINT_MAX)
	{
		Update3DSoundPosition(m_DashSE, m_Position);
	}
}

void CThornMan::StartDiscovery()
{
	PlayAnimation<EThornManAnimationNumber>(m_AnimationDataList, EThornManAnimationNumber::DISCOVERY, true);
	PlaySoundIndex(ESoundIndex::SLIME_DISCOVERY);
	StopSoundID(m_WalkSE);
}

void CThornMan::UpdateDiscovery(float DeltaTime)
{
	if (!m_Player)
	{
		return;
	}

	TowardThePlayer(DISCOVERY_ROTATION_SPEED, DeltaTime);
	if (IsAnimationEnd<EThornManAnimationNumber>(EThornManAnimationNumber::DISCOVERY))
	{
		m_StateMachine.ChangeState(EThornManState::PLAYER_MOVE);
	}
}

void CThornMan::StartDeath()
{
	PlayAnimation<EThornManAnimationNumber>(m_AnimationDataList, EThornManAnimationNumber::DEATH, true);
}

void CThornMan::UpdateDeath(float DeltaTime)
{
	if (!IsAnimationEnd<EThornManAnimationNumber>(EThornManAnimationNumber::DEATH))
	{
		return;
	}

	ItemDrop(m_ItemDropData.ItemDropType, m_ItemDropData.Percent);
	CreateDeathEffect(Vector3(m_Position.x, m_Position.y + ADD_DEATH_EFFECT_HEIGHT, m_Position.z), DEATH_EFFECT_SIZE);
	if (m_IsRespawn)
	{
		m_StateMachine.ChangeState(EThornManState::RESPAWN_WAIT);
	}
	else
	{
		m_IsDestroy = true;
	}
}

void CThornMan::UpdateRespawnWait(float DeltaTime)
{
	if (m_RespawnFrameTime > m_StateMachine.GetCurrentStateTime())
	{
		return;
	}

	RespawnSE();
	m_HP.SetHP(LIFE);
	m_NowTargetPoint = 0;
	m_StateMachine.ChangeState(EThornManState::IDLE);
	m_CurrentSpeed = 0.0f;
	m_Position = m_FirstPosition;
	if (m_BodySphere)
	{
		m_BodySphere->Update(m_Position);
	}
	m_Scale = m_FirstScale;
	m_Rotation = m_FirstRotation;
	UpdateWorldMatrix();
}