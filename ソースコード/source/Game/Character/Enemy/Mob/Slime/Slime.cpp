#include "Slime.h"
#include "../../../Player/Player.h"
#include "../../../../../Framework/Collision/Sphere.h"
#include "../../../../../Framework/Collision/Box.h"
#include "../../../../../Framework/Collision/Collision.h"
#include "../../../../../Framework/Scene/SceneManager.h"
#include "../../../../../Framework/Scene/Scene.h"
#include "../../../../../Framework/Object/CommonObject.h"
#include "../../../../../Framework/Sound/SoundTypes.h"
#include "../../../../../Framework/Input/InputInterface.h"
#include "../../../../../Framework/HitStop/HitStop.h"
#include "../../../../../Framework/Shadow/ShadowType.h"
#include "../../../../Effect/EffectTypes.h"

namespace
{
	constexpr int LIFE = 1;
	constexpr int PLAYER_DAMAGE = 1;
	constexpr float CULLING_RADIUS = 3.0f;
	constexpr float WAIT_ROTATION_SPEED = 3.0f;
	constexpr float DISCOVERY_ROTATION_SPEED = 3.0f;
	constexpr float WAIT_TIME = 2.0f;
	constexpr float KNOCKBACK_TIME = 0.35f;
	constexpr float KNOCKBACK_POWER = 0.77f;
	constexpr float KNOCKBACK_HEIGHT_POWER = 3.0f;
	constexpr float FALLATTACK_DEATH_TIME = 0.85f;
	constexpr float FALLATTACK_DEATH_SCALE_Y = 1.3f;
	constexpr float FALLATTACK_DEATH_SCALE_TIME = 0.3f;
	constexpr float ADD_HIT_EFFECT_HEIGHT = 2.5f;
	constexpr float HIT_EFFECT_SIZE = 5.5f;
	constexpr float ADD_DEATH_EFFECT_HEIGHT = 1.5f;
	constexpr float DEATH_EFFECT_SIZE = 5.5f;
};

CSlime::CSlime(Vector3 pos)
{
	m_AnimationDataList =
	{
		{ ESlimeAnimationNumber::IDLE,			AnimationData(0.5f, true)  },
		{ ESlimeAnimationNumber::WALK,			AnimationData(0.3f, true)  },
		{ ESlimeAnimationNumber::DISCOVERY,		AnimationData(0.5f, false) },
		{ ESlimeAnimationNumber::DEATH,			AnimationData(0.5f, true)  },
	};

	m_SpeedParameterList =
	{
		{ ESlimeSpeedParameterType::WALK, SpeedParameter(0.0f, 0.05f, 3.0f, 2.0f, 8.0f) },
		{ ESlimeSpeedParameterType::PLAYER_MOVE, SpeedParameter(0.0f, 0.09f, 8.0f, 1.0f, 10.0f) },
		{ ESlimeSpeedParameterType::KNOCKBACK_DEATH, SpeedParameter(0.0f, 0.2f, 10.0f, 1.0f, 0.0f) },
	};

	m_StateMachine.AddState<CSlime>(
		 ESlimeState::IDLE,
		this,
		&CSlime::StartIdle,
		&CSlime::UpdateIdle,
		nullptr
	);
	m_StateMachine.AddState<CSlime>(
		 ESlimeState::WALK,
		this,
		&CSlime::StartWalk,
		&CSlime::UpdateWalk,
		nullptr
	);
	m_StateMachine.AddState<CSlime>(
		 ESlimeState::PLAYER_MOVE,
		this,
		&CSlime::StartPlayerMove,
		&CSlime::UpdatePlayerMove,
		nullptr
	);
	m_StateMachine.AddState<CSlime>(
		 ESlimeState::DISCOVERY,
		this,
		&CSlime::StartDiscovery,
		&CSlime::UpdateDiscovery,
		nullptr
	);
	m_StateMachine.AddState<CSlime>(
		 ESlimeState::DEATH,
		this,
		&CSlime::StartDeath,
		&CSlime::UpdateDeath,
		nullptr
	);
	m_StateMachine.AddState<CSlime>(
		ESlimeState::RESPAWN_WAIT,
		this,
		nullptr,
		&CSlime::UpdateRespawnWait,
		nullptr
	);

	m_CullingRadius = CULLING_RADIUS;
	m_EnemyType = EEnemyType::SLIME;
	m_Position = pos;
	m_FirstPosition = m_Position;
	m_Scale = Vector3(10, 10, 10);
	SetRotationYaw(180.0f);
	m_FirstRotation = m_Rotation;
	m_FirstScale = m_Scale;
	m_HP.SetMaxHP(LIFE);
	m_HP.SetHP(LIFE);
	CreateVector();
	m_SkeletalMesh = std::make_unique<CSkeletalMesh>(ESkeletalMeshType::SLIME);
	//RangeSphereÇÕíTímîÕàÕÇ»ÇÃÇ≈èâä˙ílÇ©ÇÁìÆÇ©Ç≥Ç»Ç¢
	m_RangeSphere = std::make_unique<CSphere>(m_FirstPosition, 15.0f);
	if (m_RangeSphere)
	{
		m_RangeSphere->SetAddSpherePosition(Vector3(0, 2, 0));
		m_RangeSphere->Update(m_FirstPosition);
	}
	m_RangeBox = std::make_unique<CBox>(m_FirstPosition, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	if (m_RangeBox)
	{
		m_RangeBox->AddBoxPosition(Vector3(0.0f, 0.0f, 0.0f));
		m_RangeBox->Update(m_FirstPosition, Vector3::Zero());
	}
	m_BodySphere = std::make_unique<CSphere>(m_FirstPosition, 0.5f);
	if (m_BodySphere)
	{
		m_BodySphere->SetAddSpherePosition(Vector3(0, 0.5f, 0));
		m_BodySphere->Update(m_FirstPosition);
	}
	m_StateMachine.SetInitialState(ESlimeState::IDLE);
}

void CSlime::Update(float DeltaTime)
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
		if (m_RangeSphere)
		{
			m_RangeSphere->Update(m_FirstPosition);
		}
		if (m_RangeBox)
		{
			m_RangeBox->Update(m_FirstPosition, Vector3::Zero());
		}
		if (m_BodySphere)
		{
			m_BodySphere->Update(m_Position);
		}
		UpdateWorldMatrix();
		return;
	}

	if (m_StateMachine.GetState() == ESlimeState::RESPAWN_WAIT)
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
	
	if (m_BodySphere)
	{
		m_BodySphere->Update(m_Position);
	}
	UpdateWorldMatrix();
}

void CSlime::Draw()
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
	if (m_RangeSphere)
	{
		m_RangeSphere->Draw();
	}
	if (m_RangeBox)
	{
		m_RangeBox->Draw();
	}
	if (m_BodySphere)
	{
		m_BodySphere->Draw();
	}
}

void CSlime::SingleCollisonDraw()
{
	if (m_RangeSphere)
	{
		m_RangeSphere->SingleDraw();
	}
	if (m_RangeBox)
	{
		m_RangeBox->SingleDraw();
	}
	if (m_BodySphere)
	{
		m_BodySphere->SingleDraw();
	}
}

void CSlime::PlayerHitCheck()
{
	PlayerHitCheckEntity(LIFE, LIFE, KNOCKBACK_POWER);
}

void CSlime::Damage(const int Damage)
{
	PlayVibration(EVibrationType::LOW);
	PlayHitStop(EHitStopType::LOW);
	StopSoundID(m_WalkSE);
	StopSoundID(m_DashSE);
	CreateHitEffect(Vector3(m_Position.x, m_Position.y + ADD_HIT_EFFECT_HEIGHT, m_Position.z), HIT_EFFECT_SIZE);
	m_HP.AddDamage(Damage);
	m_StateMachine.ChangeState(ESlimeState::DEATH);
}

void CSlime::BodyCollisionCheck(const float DeltaTime)
{
	if (IsRespawnWait())
	{
		return;
	}

	UpdateEnemyCollision();
	UpdateMeshVsBodyCollision();
	if (!m_IsKnockBackDeath)
	{
		UpdateGroundCollision(DeltaTime);
	}
}

void CSlime::SetBodySphere(const Vector3 local_pos, const float radius)
{
	if (m_BodySphere)
	{
		m_BodySphere->SetAddSpherePosition(local_pos);
		m_BodySphere->SetRadius(radius);
		m_BodySphere->Update(m_FirstPosition);
	}
}

bool CSlime::IsRespawnWait() const
{
	return m_StateMachine.GetState() == ESlimeState::RESPAWN_WAIT;
}

void CSlime::StartIdle()
{
	PlayAnimation<ESlimeAnimationNumber>(m_AnimationDataList, ESlimeAnimationNumber::IDLE, true);
	StopSoundID(m_WalkSE);
	StopSoundID(m_DashSE);
	m_IsPlayerDiscovery = false;
}

void CSlime::UpdateIdle(float DeltaTime)
{
	if (IsPlayerRange())
	{
		m_StateMachine.ChangeState(ESlimeState::DISCOVERY);
		return;
	}
	Vector3 direction = m_TargetPoint[m_NowTargetPoint] - m_Position;
	direction.y = 0.0f;

	DirectionRotation(WAIT_ROTATION_SPEED, DeltaTime, direction);
	UpdateVector();
	if (m_StateMachine.GetCurrentStateTime() > WAIT_TIME)
	{
		m_StateMachine.ChangeState(ESlimeState::WALK);
	}
}

void CSlime::StartWalk()
{
	PlayAnimation<ESlimeAnimationNumber>(m_AnimationDataList, ESlimeAnimationNumber::WALK , true);
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

void CSlime::UpdateWalk(float DeltaTime)
{
	if (IsPlayerRange())
	{
		if (m_IsPlayerDiscovery)
		{
			m_StateMachine.ChangeState(ESlimeState::PLAYER_MOVE);	
			return;
		}
		m_StateMachine.ChangeState(ESlimeState::DISCOVERY);
		return;
	}
	if (m_MaxTargetPoint == -1 || !m_SpeedParameterList.contains(ESlimeSpeedParameterType::WALK))
	{
		return;
	}

	Vector3 direction = m_TargetPoint[m_NowTargetPoint] - m_Position;
	direction.y = 0.0f;

	SpeedParameter& speedParam = m_SpeedParameterList[ESlimeSpeedParameterType::WALK];
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
		m_StateMachine.ChangeState(ESlimeState::IDLE);
	}
}

void CSlime::StartPlayerMove()
{
	PlayAnimation<ESlimeAnimationNumber>(m_AnimationDataList, ESlimeAnimationNumber::WALK, true);
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

void CSlime::UpdatePlayerMove(float DeltaTime)
{
	if (!IsPlayerRange())
	{
		m_StateMachine.ChangeState(ESlimeState::WALK);
		return;
	}
	
	if (!m_Player)
	{
		return;
	}
	if (m_Player->IsDamageOrDeath())
	{
		ResetSpeed();
		return;
	}

	PlayerMove(m_SpeedParameterList[ESlimeSpeedParameterType::PLAYER_MOVE], DeltaTime);
	if (m_DashSE != UINT_MAX)
	{
		Update3DSoundPosition(m_DashSE, m_Position);
	}
}

void CSlime::StartDiscovery()
{
	PlayAnimation<ESlimeAnimationNumber>(m_AnimationDataList, ESlimeAnimationNumber::DISCOVERY, true);
	PlaySoundIndex(ESoundIndex::SLIME_DISCOVERY);
	StopSoundID(m_WalkSE);
	m_IsPlayerDiscovery = true;
}

void CSlime::UpdateDiscovery(float DeltaTime)
{
	if (!m_Player)
	{
		return;
	}

	TowardThePlayer(DISCOVERY_ROTATION_SPEED, DeltaTime);
	if (IsAnimationEnd<ESlimeAnimationNumber>(ESlimeAnimationNumber::DISCOVERY))
	{
		m_StateMachine.ChangeState(ESlimeState::PLAYER_MOVE);
	}
}

void CSlime::StartDeath()
{
	PlayAnimation<ESlimeAnimationNumber>(m_AnimationDataList, ESlimeAnimationNumber::DEATH, true);
}

void CSlime::UpdateDeath(float DeltaTime)
{
	bool isEnd = false;
	if (m_IsKnockBackDeath)
	{
		isEnd = UpdateKnockBackDeathMove(m_StateMachine.GetCurrentStateTime(), DeltaTime, KNOCKBACK_TIME, KNOCKBACK_HEIGHT_POWER, m_SpeedParameterList[ESlimeSpeedParameterType::KNOCKBACK_DEATH]);
	}
	else
	{
		isEnd = UpdateFallAttackDeathMove(m_StateMachine.GetCurrentStateTime(), FALLATTACK_DEATH_SCALE_TIME, FALLATTACK_DEATH_SCALE_Y, FALLATTACK_DEATH_TIME);
	}

	if (!isEnd)
	{
		return;
	}

	m_IsKnockBackDeath = false;
	ItemDrop(m_ItemDropData.ItemDropType, m_ItemDropData.Percent);
	CreateDeathEffect(Vector3(m_Position.x, m_Position.y + ADD_DEATH_EFFECT_HEIGHT, m_Position.z), DEATH_EFFECT_SIZE);
	if (m_IsRespawn)
	{
		m_StateMachine.ChangeState(ESlimeState::RESPAWN_WAIT);
	}
	else
	{
		m_IsDestroy = true;
	}
}

void CSlime::UpdateRespawnWait(float DeltaTime)
{
	if (m_RespawnFrameTime > m_StateMachine.GetCurrentStateTime())
	{
		return;
	}

	RespawnSE();
	m_HP.SetHP(LIFE);
	m_NowTargetPoint = 0;
	m_StateMachine.ChangeState(ESlimeState::IDLE);
	m_Position = m_FirstPosition;
	if (m_BodySphere)
	{
		m_BodySphere->Update(m_Position);
	}
	m_Scale = m_FirstScale;
	m_Rotation = m_FirstRotation;
	m_CurrentSpeed = 0.0f;
	UpdateWorldMatrix();
}
