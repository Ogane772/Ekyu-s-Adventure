#include "Monkey.h"
#include "../../../../../Framework/Collision/Sphere.h"
#include "../../../../../Framework/Collision/Box.h"
#include "../../../../../Framework/Collision/Capsule.h"
#include "../../../../../Framework/Collision/Collision.h"
#include "../../../../../Framework/Scene/SceneManager.h"
#include "../../../../../Framework/Scene/Scene.h"
#include "../../../../../Framework/Sound/SoundTypes.h"
#include "../../../../../Framework/Line/Line.h"
#include "../../../../../Framework/Input/InputInterface.h"
#include "../../../../../Framework/HitStop/HitStop.h"
#include "../../../../../Framework/Shadow/ShadowType.h"
#include "../../../../Effect/EffectTypes.h"
#include "../../../Player/Player.h"
#include "../../EnemyObject/Bom.h"

namespace
{
	constexpr int LIFE = 1;
	constexpr int PLAYER_DAMAGE = 1;
	constexpr float CULLING_RADIUS = 6.0f;
	constexpr float WAIT_ROTATION_SPEED = 3.0f;
	constexpr float DISCOVERY_ROTATION_SPEED = 5.0f;
	constexpr float THROW_ROTATION_SPEED = 8.0f;
	constexpr float START_THROW_TIME = 1.5f;
	constexpr float BOM_CREATE_TIME = 0.08f;
	constexpr float BOM_MOVE_END_TIME = 0.4f;
	constexpr float START_BOM_THROW_TIME = 0.47f;
	constexpr float KNOCKBACK_TIME = 0.5f;
	constexpr float KNOCKBACK_POWER = 0.77f;
	constexpr float KNOCKBACK_HEIGHT_POWER = 3.0f;
	constexpr float FALLATTACK_DEATH_TIME = 0.85f;
	constexpr float FALLATTACK_DEATH_SCALE_Y = 1.5f;
	constexpr float FALLATTACK_DEATH_SCALE_TIME = 0.4f;
	constexpr float ADD_HIT_EFFECT_HEIGHT = 2.5f;
	constexpr float HIT_EFFECT_SIZE = 5.5f;
	constexpr float ADD_DEATH_EFFECT_HEIGHT = 1.5f;
	constexpr float DEATH_EFFECT_SIZE = 5.5f;
	constexpr float ADD_INIT_BOM_POSITION_Y = 2.0f;
	constexpr float ADD_BOM_HEIGHT_POWER = 3.0f;
};

CMonkey::CMonkey(Vector3 pos)
{
	m_AnimationDataList =
	{
		{ EMonkeyAnimationNumber::IDLE,					AnimationData(0.4f, true)  },
		{ EMonkeyAnimationNumber::DISCOVERY_START,		AnimationData(0.56f, false)  },
		{ EMonkeyAnimationNumber::DISCOVERY_LOOP,		AnimationData(0.4f, true) },
		{ EMonkeyAnimationNumber::DISCOVERY_END,		AnimationData(0.5f, false)  },
		{ EMonkeyAnimationNumber::THROW,				AnimationData(0.4f, false)  },
		{ EMonkeyAnimationNumber::DEATH_MOVE,			AnimationData(0.5f, false)  },
		{ EMonkeyAnimationNumber::DEATH_DIVE,			AnimationData(1.0f, false)  },
	};

	m_ThrowTimeScheduler.Add(BOM_CREATE_TIME, [=, this]()
	{
		std::weak_ptr<CScene> scene = GetScene();
		if (scene.expired())
		{
			return;
		}
		m_Bom = scene.lock()->CreateAddGameObject<CBom>(ESceneObjectType::BILLBOARD);
		if (m_Bom)
		{
			m_Bom->Init(Vector3(m_Position.x + m_Front.x, m_Position.y + ADD_INIT_BOM_POSITION_Y, m_Position.z + m_Front.z));
		}
		PlaySoundIndex(ESoundIndex::BOM_START);
	});

	m_ThrowTimeScheduler.Add(START_BOM_THROW_TIME, [=, this]()
	{
		if (m_Bom)
		{
			m_Bom->ChangeMoveState();
			//後はシーン側の管理にまかせるためNULLにする
			m_Bom = nullptr;
		}
	});

	m_StateMachine.AddState<CMonkey>(
		EMonkeyState::IDLE,
		this,
		&CMonkey::StartIdle,
		&CMonkey::UpdateIdle,
		nullptr
	);
	m_StateMachine.AddState<CMonkey>(
		EMonkeyState::DISCOVERY_START,
		this,
		&CMonkey::StartDiscoveryStart,
		&CMonkey::UpdateDiscoveryStart,
		nullptr
	);
	m_StateMachine.AddState<CMonkey>(
		EMonkeyState::DISCOVERY_LOOP,
		this,
		&CMonkey::StartDiscoveryLoop,
		&CMonkey::UpdateDiscoveryLoop,
		nullptr
	);
	m_StateMachine.AddState<CMonkey>(
		EMonkeyState::DISCOVERY_END,
		this,
		&CMonkey::StartDiscoveryEnd,
		&CMonkey::UpdateDiscoveryEnd,
		nullptr
	);
	m_StateMachine.AddState<CMonkey>(
		EMonkeyState::THROW,
		this,
		&CMonkey::StartThrow,
		&CMonkey::UpdateThrow,
		nullptr
	);
	m_StateMachine.AddState<CMonkey>(
		EMonkeyState::DEATH,
		this,
		&CMonkey::StartDeath,
		& CMonkey::UpdateDeath,
		nullptr
	);
	m_StateMachine.AddState<CMonkey>(
		EMonkeyState::RESPAWN_WAIT,
		this,
		nullptr,
		&CMonkey::UpdateRespawnWait,
		nullptr
	);
	m_KnockBackSpeedParameter = SpeedParameter(0.0f, 0.2f, 10.0f, 1.0f, 0.0f);

	m_CullingRadius = CULLING_RADIUS;
	m_EnemyType = EEnemyType::MONKEY;
	m_Position = pos;
	m_FirstPosition = m_Position;
	m_Scale = Vector3(10, 10, 10);
	m_Rotation = Vector3(0, 180, 0);
	m_FirstRotation = m_Rotation;
	m_FirstScale = m_Scale;
	m_DefaultRotation = m_Rotation;
	m_HP.SetMaxHP(LIFE);
	m_HP.SetHP(LIFE);
	CreateVector();
	m_SkeletalMesh = std::make_unique<CSkeletalMesh>(ESkeletalMeshType::MONKEY);
	//RangeSphereは探知範囲なので初期値から動かさない
	m_RangeSphere = std::make_unique<CSphere>(m_FirstPosition, 15.0f);
	if (m_RangeSphere)
	{
		m_RangeSphere->SetAddSpherePosition(Vector3(0, 2, 0));
	}
	m_BodyCapsule = std::make_unique<CCapsule>(Vector3(0, 0.8f, 0), Vector3(0, 2.7f, 0.0), Vector3(0.0, 0.0f, 0.1f), 0.75f, 8, 8);
	m_StateMachine.SetInitialState(EMonkeyState::IDLE);
}

void CMonkey::Update(float DeltaTime)
{
	if (IsEvent())
	{
		return;
	}
	if (IsEditMode())
	{
		m_IsCulling = true;
		m_BodyCapsule->Update(m_Position);
		m_RangeSphere->Update(m_Position);
		m_FirstRotation = m_Rotation;
		m_FirstScale = m_Scale;
		UpdateWorldMatrix();
		return;
	}
	if (m_StateMachine.GetState() == EMonkeyState::RESPAWN_WAIT)
	{
		m_StateMachine.Update(DeltaTime);
		return;
	}

	if (m_SkeletalMesh)
	{
		m_SkeletalMesh->Update(DeltaTime);
	}

	m_StateMachine.Update(DeltaTime);
	
	if (m_BodyCapsule)
	{
		m_BodyCapsule->Update(m_Position);
	}

	UpdateWorldMatrix();
}

void CMonkey::Draw()
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
	if (m_BodyCapsule)
	{
		m_BodyCapsule->Draw();
	}
	if (m_RangeSphere)
	{
		m_RangeSphere->Draw();
	}
}

void CMonkey::SingleCollisonDraw()
{
	if (m_BodyCapsule)
	{
		m_BodyCapsule->SingleDraw();
	}
	if(m_RangeSphere)
	{ 
		m_RangeSphere->SingleDraw();
	}
}

void CMonkey::PlayerHitCheck()
{
	PlayerHitCheckEntity(LIFE, LIFE, KNOCKBACK_POWER);
}

void CMonkey::Damage(int Damage)
{
	PlayVibration(EVibrationType::LOW);
	PlayHitStop(EHitStopType::LOW);
	CreateHitEffect(Vector3(m_Position.x, m_Position.y + ADD_HIT_EFFECT_HEIGHT, m_Position.z), HIT_EFFECT_SIZE);
	m_HP.AddDamage(Damage);
	m_StateMachine.ChangeState(EMonkeyState::DEATH);
}

CCapsule* CMonkey::GetCapsule() const
{
	return m_BodyCapsule.get();
}

void CMonkey::DestroyNormalBom()
{
	if (!m_Bom)
	{
		return;
	}
	if (!m_Bom->IsMoveState())
	{
		m_Bom->DestroyBillboard();
		m_Bom->SetDestroy();
		m_Bom = nullptr;
	}
}

void CMonkey::StartIdle()
{
	PlayAnimation<EMonkeyAnimationNumber>(m_AnimationDataList, EMonkeyAnimationNumber::IDLE, false);
}

void CMonkey::UpdateIdle(float DeltaTime)
{
	if (IsPlayerRange())
	{
		m_StateMachine.ChangeState(EMonkeyState::DISCOVERY_START);
		return;
	}
	TargetRotation(WAIT_ROTATION_SPEED, DeltaTime, m_DefaultRotation);
	UpdateVector();
}

void CMonkey::StartDiscoveryStart()
{
	PlayAnimation<EMonkeyAnimationNumber>(m_AnimationDataList, EMonkeyAnimationNumber::DISCOVERY_START, true);
}

void CMonkey::UpdateDiscoveryStart(float DeltaTime)
{
	if (!m_Player)
	{
		return;
	}
	TowardThePlayer(DISCOVERY_ROTATION_SPEED, DeltaTime);
	if (IsAnimationEnd<EMonkeyAnimationNumber>(EMonkeyAnimationNumber::DISCOVERY_START))
	{
		m_StateMachine.ChangeState(EMonkeyState::DISCOVERY_LOOP);
	}
}

void CMonkey::StartDiscoveryLoop()
{
	PlayAnimation<EMonkeyAnimationNumber>(m_AnimationDataList, EMonkeyAnimationNumber::DISCOVERY_LOOP, false);
}

void CMonkey::UpdateDiscoveryLoop(float DeltaTime)
{
	if (!IsPlayerRange())
	{
		m_StateMachine.ChangeState(EMonkeyState::DISCOVERY_END);
		return;
	}

	TowardThePlayer(DISCOVERY_ROTATION_SPEED, DeltaTime);

	if (m_StateMachine.GetCurrentStateTime() > START_THROW_TIME)
	{
		m_StateMachine.ChangeState(EMonkeyState::THROW);
	}
}

void CMonkey::StartDiscoveryEnd()
{
	PlayAnimation<EMonkeyAnimationNumber>(m_AnimationDataList, EMonkeyAnimationNumber::DISCOVERY_END);
}

void CMonkey::UpdateDiscoveryEnd(float DeltaTime)
{
	if (IsPlayerRange())
	{
		m_StateMachine.ChangeState(EMonkeyState::DISCOVERY_LOOP);
		return;
	}
	if (IsAnimationEnd<EMonkeyAnimationNumber>(EMonkeyAnimationNumber::DISCOVERY_END))
	{
		m_StateMachine.ChangeState(EMonkeyState::IDLE);
	}
}

void CMonkey::StartThrow()
{
	PlayAnimation<EMonkeyAnimationNumber>(m_AnimationDataList, EMonkeyAnimationNumber::THROW, true);
	m_ThrowTimeScheduler.Reset();
}

void CMonkey::UpdateThrow(float DeltaTime)
{
	if (IsAnimationEnd<EMonkeyAnimationNumber>(EMonkeyAnimationNumber::THROW))
	{
		m_StateMachine.ChangeState(EMonkeyState::DISCOVERY_LOOP);
		return;
	}

	m_ThrowTimeScheduler.Run(DeltaTime);

	const float currentTime = m_StateMachine.GetCurrentStateTime();
	if(currentTime > BOM_CREATE_TIME && currentTime < BOM_MOVE_END_TIME)
	{
		if (m_Bom)
		{
			const float bomY = m_Bom->GetPosition().y + ADD_BOM_HEIGHT_POWER * DeltaTime;
			Vector3 bomPosition = m_Position + m_Front;
			bomPosition.y = bomY;
			m_Bom->SetPosition(bomPosition);
		}
	}

	if (!m_ThrowTimeScheduler.IsEndTask())
	{
		TowardThePlayer(THROW_ROTATION_SPEED, DeltaTime);
	}
}

void CMonkey::StartDeath()
{
	if (m_IsKnockBackDeath)
	{
		PlayAnimation<EMonkeyAnimationNumber>(m_AnimationDataList, EMonkeyAnimationNumber::DEATH_DIVE, true);
	}
	else
	{
		PlayAnimation<EMonkeyAnimationNumber>(m_AnimationDataList, EMonkeyAnimationNumber::DEATH_MOVE, true);
	}
	DestroyNormalBom();
}

void CMonkey::UpdateDeath(float DeltaTime)
{
	bool isEnd = false;
	if (m_IsKnockBackDeath)
	{
		isEnd = UpdateKnockBackDeathMove(m_StateMachine.GetCurrentStateTime(), DeltaTime, KNOCKBACK_TIME, KNOCKBACK_HEIGHT_POWER, m_KnockBackSpeedParameter);
		UpdateMeshVsBodyCollision();
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
		m_StateMachine.ChangeState(EMonkeyState::RESPAWN_WAIT);
	}
	else
	{
		m_IsDestroy = true;
	}
}

void CMonkey::UpdateRespawnWait(float DeltaTime)
{
	if (m_RespawnFrameTime > m_StateMachine.GetCurrentStateTime())
	{
		return;
	}

	RespawnSE();
	m_HP.SetHP(LIFE);
	m_CurrentSpeed = 0.0f;
	m_StateMachine.ChangeState(EMonkeyState::IDLE);
	m_Position = m_FirstPosition;
	m_Scale = m_FirstScale;
	m_Rotation = m_FirstRotation;
	UpdateWorldMatrix();
}

bool CMonkey::IsRespawnWait() const
{
	return m_StateMachine.GetState() == EMonkeyState::RESPAWN_WAIT;
}