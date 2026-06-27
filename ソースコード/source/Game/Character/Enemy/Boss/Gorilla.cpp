#include "Gorilla.h"
#include "../../Player/Player.h"
#include "../../../Effect/EffectTypes.h"
#include "../../../Object/Arrow/Arrow.h"
#include "../../../Object/HitBlock/HitBlock.h"
#include "../../../Object/ItemObject/Star.h"
#include "../../../../Framework/Camera/Camera.h"
#include "../../../../Framework/Collision/Sphere.h"
#include "../../../../Framework/Collision/Capsule.h"
#include "../../../../Framework/Collision/Collision.h"
#include "../../../../Framework/Input/InputInterface.h"
#include "../../../../Framework/Scene/SceneManager.h"
#include "../../../../Framework/Scene/Scene.h"
#include "../../../../Framework/Sound/SoundTypes.h"
#include "../../../../Framework/Object/CommonObject.h"
#include "../../../../Framework/Line/Line.h"
#include "../../../../Framework/HitStop/HitStop.h"
#include "../../../../Framework/Shadow/ShadowType.h"

namespace
{
	constexpr int LIFE = 3;
	constexpr int FIRST_DAMAGE_JUMP_ATTACK_COUNT = 2;//1度ダメージを受けた後のジャンプ攻撃回数
	constexpr int SECOND_DAMAGE_JUMP_ATTACK_COUNT = 3;//2度ダメージを受けた後のジャンプ攻撃回数
	constexpr int MY_DAMAGE = 1;
	constexpr int PLAYER_DAMAGE = 1;
	constexpr float ATTACK_START_TIME = 3.0f;
	constexpr float PLAYER_MOVE_TIME = 5.0f;
	constexpr float JUMP_ATTACK_MOVE_TIME = 3.0f;
	constexpr float JUMP_ATTACK_WAIT_TIME = 1.0f;
	constexpr float JUMP_ATTACK_UP_SPEED = 60.0f;
	constexpr float JUMP_ATTACK_DOWN_SPEED = 120.0f;
	constexpr float JUMP_ATTACK_HEIGHT = 50.0f;
	constexpr float WAIT_ROTATION_SPEED = 5.0f;
	constexpr float JUMP_ATTACK_FALL_ROTATION_SPEED = 15.0f;
	constexpr float HIT_EFFECT_SIZE = 5.5f;
	constexpr float ADD_HIT_EFFECT_HEIGHT = 2.5f;
	constexpr float DORAMING_END_TIME = 5.0f;
	constexpr float DORAMING_CAMERA_LENGHTH = 13.0f;
	constexpr float ADD_DORAMING_CAMERA_HEIGHT = 2.0f;
	constexpr float ATTACK_SPHERE_RADIUS = 2.0f;
	constexpr float ATTACK_SPHERE_FRONT_RATE = 3.0f;
};

CGorilla::CGorilla(Vector3 pos)
{
	m_AnimationDataList =
	{
		{ EGorillaAnimationNumber::IDLE,				AnimationData(0.4f, true)   },
		{ EGorillaAnimationNumber::DRUMMING,			AnimationData(0.75f,true)   },
		{ EGorillaAnimationNumber::DEATH,				AnimationData(0.45f,false)  },
		{ EGorillaAnimationNumber::ATTACK_START,		AnimationData(0.5f, false)  },
		{ EGorillaAnimationNumber::ATTACK_LOOP,			AnimationData(0.92f,true)   },
		{ EGorillaAnimationNumber::ATTACK_HITEND,		AnimationData(0.3f, false)  },
		{ EGorillaAnimationNumber::ATTACK_HITDAMAGE,	AnimationData(0.4f, false)  },
		{ EGorillaAnimationNumber::ATTACK_HIT_START,	AnimationData(0.3f, false)  },
		{ EGorillaAnimationNumber::ATTACK_HIT_LOOP,		AnimationData(0.4f, true)   },
		{ EGorillaAnimationNumber::ATTACK_END,			AnimationData(0.8f, false)  },
		{ EGorillaAnimationNumber::ATTACK_JUMP_START,	AnimationData(0.5f, false)  },
		{ EGorillaAnimationNumber::ATTACK_JUMP_LOOP,	AnimationData(0.5f, true)   },
		{ EGorillaAnimationNumber::ATTACK_JUMP_END,		AnimationData(0.75f,false)  },
		{ EGorillaAnimationNumber::JUMP_GUARD,			AnimationData(0.5f, false)   },
	};

	m_SpeedParameterList =
	{
		{ EGorillaSpeedParameterType::PLAYER_MOVE, SpeedParameter(0.0f, 0.21f, 1.0f, 1.0f, 5.0f) },
		{ EGorillaSpeedParameterType::FIRST_DAMAGE_PLAYER_MOVE, SpeedParameter(0.0f, 0.23f, 3.0f, 1.0f, 7.0f) },
		{ EGorillaSpeedParameterType::FIRST_DAMAGE_PLAYER_MOVE, SpeedParameter(0.0f, 0.25f, 8.0f, 1.0f, 10.0f) },
		{ EGorillaSpeedParameterType::JUMP_ATTACK, SpeedParameter(0.0f, 0.19f, 1.0f, 1.0f, 20.0f) },
		{ EGorillaSpeedParameterType::FIRST_DAMAGE_JUMP_ATTACK, SpeedParameter(0.0f, 0.2f, 3.0f, 1.0f, 20.0f) },
		{ EGorillaSpeedParameterType::SECOND_DAMAGE_JUMP_ATTACK, SpeedParameter(0.0f, 0.23f, 5.0f, 1.0f, 20.0f) },
	};

	m_LifeJumpAttackList =
	{
		{ LIFE, 0 },
		{ LIFE - 1, FIRST_DAMAGE_JUMP_ATTACK_COUNT  },
		{ LIFE - 2, SECOND_DAMAGE_JUMP_ATTACK_COUNT },
	};

	m_LifePlayerMoveSpeedList =
	{
		{ LIFE, m_SpeedParameterList[EGorillaSpeedParameterType::PLAYER_MOVE] },
		{ LIFE - 1, m_SpeedParameterList[EGorillaSpeedParameterType::FIRST_DAMAGE_PLAYER_MOVE] },
		{ LIFE - 2, m_SpeedParameterList[EGorillaSpeedParameterType::SECOND_DAMAGE_PLAYER_MOVE] },
	};

	m_LifeJumpAttackSpeedList =
	{
		{ LIFE, m_SpeedParameterList[EGorillaSpeedParameterType::JUMP_ATTACK] },
		{ LIFE - 1, m_SpeedParameterList[EGorillaSpeedParameterType::FIRST_DAMAGE_JUMP_ATTACK] },
		{ LIFE - 2, m_SpeedParameterList[EGorillaSpeedParameterType::SECOND_DAMAGE_JUMP_ATTACK] },
	};

	m_StateMachine.AddState<CGorilla>(
		EGorillaState::DRUMMING,
		this,
		&CGorilla::StartDrumming,
		&CGorilla::UpdateDrumming,
		nullptr
	);
	m_StateMachine.AddState<CGorilla>(
		EGorillaState::IDLE,
		this,
		&CGorilla::StartIdle,
		&CGorilla::UpdateIdle,
		nullptr
	);
	m_StateMachine.AddState<CGorilla>(
		EGorillaState::PLAYER_MOVE,
		this,
		&CGorilla::StartPlayerMove,
		&CGorilla::UpdatePlayerMove,
		nullptr
	);
	m_StateMachine.AddState<CGorilla>(
		EGorillaState::JUMP_ATTACK,
		this,
		&CGorilla::StartJumpAttack,
		&CGorilla::UpdateJumpAttack,
		nullptr
	);
	m_StateMachine.AddState<CGorilla>(
		EGorillaState::STUN,
		this,
		&CGorilla::StartStun,
		&CGorilla::UpdateStun,
		nullptr
	);
	m_StateMachine.AddState<CGorilla>(
		EGorillaState::DAMAGE,
		this,
		&CGorilla::StartDamage,
		&CGorilla::UpdateDamage,
		nullptr
	);
	m_StateMachine.AddState<CGorilla>(
		EGorillaState::DEATH,
		this,
		&CGorilla::StartDeath,
		&CGorilla::UpdateDeath,
		nullptr
	);

	m_JumpAttackStateMachine.AddState<CGorilla>(
		EJumpAttackState::START,
		this,
		&CGorilla::StartJumpAttackStart,
		&CGorilla::UpdateJumpAttackStart,
		nullptr
	);
	m_JumpAttackStateMachine.AddState<CGorilla>(
		EJumpAttackState::LOOP,
		this,
		&CGorilla::StartJumpAttackLoop,
		&CGorilla::UpdateJumpAttackLoop,
		nullptr
	);
	m_JumpAttackStateMachine.AddState<CGorilla>(
		EJumpAttackState::END,
		this,
		&CGorilla::StartJumpAttackEnd,
		&CGorilla::UpdateJumpAttackEnd,
		nullptr
	);
	m_JumpAttackStateMachine.AddState<CGorilla>(
		EJumpAttackState::WAIT,
		this,
		nullptr,
		&CGorilla::UpdateJumpAttackWait,
		nullptr
	);

	m_IsNoCullingCheck = true;
	m_EnemyType = EEnemyType::GORILLA;
	m_Position = pos;
	m_FirstPosition = m_Position;
	m_Scale = Vector3(18, 18, 18);
	m_Rotation = Vector3(0, 180, 0);
	m_HP.SetMaxHP(LIFE);
	m_HP.SetHP(LIFE);
	CreateVector();
	m_SkeletalMesh = std::make_unique<CSkeletalMesh>(ESkeletalMeshType::GORILLA);
	m_BodyCapsule = std::make_unique<CCapsule>(Vector3(0, 1.8f, 0), Vector3(0, 4.0f, 0.0), Vector3(0.0, 0.0f, 0.1f), 2.0f, 8, 8);
	if (m_BodyCapsule)
	{
		m_BodyCapsule->Update(m_Position);
	}
	m_AttackSphere = std::make_unique<CSphere>(m_Position, 0.0f);
	m_StateMachine.SetInitialState(EGorillaState::DRUMMING);
}

void CGorilla::Update(float DeltaTime)
{
	if (IsEditMode())
	{
		UpdateWorldMatrix();
		return;
	}

	if (m_SkeletalMesh)
	{
		m_SkeletalMesh->Update(DeltaTime);
	}
	m_StateMachine.Update(DeltaTime);
	BodyCollisionCheck();
	
	if (m_BodyCapsule)
	{
		m_BodyCapsule->Update(m_Position + (m_Front * 0.5f));
	}

	UpdateWorldMatrix();
}

void CGorilla::BodyCollisionCheck()
{
	if (m_StateMachine.GetState() == EGorillaState::PLAYER_MOVE)
	{
		std::weak_ptr<CScene> scene = GetScene();
		if (scene.expired())
		{
			return;
		}
		//ヒットブロックとの判定
		std::vector<CHitBlock*> blocks;
		scene.lock()->GetHitBlockObjects(ESceneObjectType::MESH_OBJECT, blocks);
		for (CHitBlock* block : blocks)
		{
			if (!block)
			{
				continue;
			}
			if(CCollison::CollisionCheck(m_BodyCapsule.get(), block->GetBox()))
			{
				PlayEffect(EEffectType::WALL_HIT, Vector3(m_Position.x + m_Front.x, m_Position.y + 5.1f, m_Position.z + m_Front.z), 5.5f);
				block->SetHit(m_Front);
				DeleteSoundIndex(ESoundIndex::SWING);
				PlayHitStop(EHitStopType::MIDDLE);
				PlayVibration(EVibrationType::MIDDLE);
				PlayCameraShake(ECameraShakeType::HIGH);
				m_StateMachine.ChangeState(EGorillaState::STUN);
				PlaySoundIndex(ESoundIndex::GORILLA_WALL_HIT);
			}
		}
	}
}

void CGorilla::ChoiceNextAttackState()
{
	//最大HPならプレイヤーを追尾し、それ以外なら指定回数分ジャンプ攻撃後、プレイヤーを追尾する
	const int currentHP = m_HP.GetCurrentHP();
	if(!m_LifeJumpAttackList.contains(currentHP))
	{
		return;
	}

	const int lifeJumpAttackCount = m_LifeJumpAttackList[currentHP];
	if (m_JumpAttackCount < lifeJumpAttackCount)
	{
		m_StateMachine.ChangeState(EGorillaState::JUMP_ATTACK);
	}
	else
	{;
		m_StateMachine.ChangeState(EGorillaState::PLAYER_MOVE);
	}
}

void CGorilla::Draw()
{
	if (m_SkeletalMesh)
	{
		m_SkeletalMesh->ShadowOnModelDraw(EShadowType::DOWN_SHADOW, m_World);
	}
	if (m_BodyCapsule)
	{
		m_BodyCapsule->Draw();
	}
	if (m_AttackSphere)
	{
		m_AttackSphere->Draw();
	}
}

void CGorilla::PlayerHitCheck()
{
	if (!m_Player)
	{
		return;
	}

	const EGorillaState state = m_StateMachine.GetState();
	//プレイヤーが壁とゴリラに挟まれて動けなくなるのを防ぐため、ジャンプ攻撃中はプレイヤーに押し出されるようにする
	if (state == EGorillaState::JUMP_ATTACK)
	{
		float timeY = m_Position.y;
		if (CCollison::CollisionCheck(m_BodyCapsule.get(), m_Player->GetCapsule(), &m_Position))
		{
			m_Position.y = timeY;
			if (m_BodyCapsule)
			{
				m_BodyCapsule->Update(m_Position);
			}
		}
	}

	if (state != EGorillaState::STUN)
	{
		return;
	}

	if (CCollison::CollisionCheck(m_BodyCapsule.get(), m_Player->GetAttackSphere()))
	{
		if (m_Player->IsHipDropState() || m_Player->IsFall())
		{
			Damage(MY_DAMAGE);
			return;
		}
	}
}

void CGorilla::Damage(int damage)
{
	if (!m_Player)
	{
		return;
	}
	m_JumpAttackCount = 0;
	CreateHitEffect(Vector3(m_Player->GetPosition().x, m_Player->GetPosition().y + ADD_HIT_EFFECT_HEIGHT, m_Player->GetPosition().z), HIT_EFFECT_SIZE);
	m_HP.AddDamage(damage);
	if (m_Player->IsHipDropState())
	{
		m_Player->ChangeNormalJumpState(ENormalJumpState::HIGH_JUMP);
		PlaySoundIndex(ESoundIndex::BOSS_JUMP_HIT);
	}
	else
	{
		m_Player->ChangeNormalJumpState(ENormalJumpState::NO_JUMP);
		PlaySoundIndex(ESoundIndex::BOSS_JUMP_HIT);
	}

	if (IsDeath())
	{
		PlayEffect(EEffectType::ATTACK_HIT3, Vector3(m_Player->GetPosition().x, m_Player->GetPosition().y + ADD_HIT_EFFECT_HEIGHT, m_Player->GetPosition().z), HIT_EFFECT_SIZE);
		PlaySoundIndex(ESoundIndex::HIT_END);
		PlayHitStop(EHitStopType::HIGH);
		PlayVibration(EVibrationType::HIGH);
	}
	else
	{
		PlayHitStop(EHitStopType::MIDDLE);
		PlayVibration(EVibrationType::LOW);
	}

	m_StateMachine.ChangeState(EGorillaState::DAMAGE);
	if (m_StunEffectId != UINT_MAX)
	{
		StopEffect(m_StunEffectId);
	}
	StopSoundIndex(ESoundIndex::STUN);
	if (m_Arrow)
	{
		m_Arrow->SetDestroy();
	}
}

void CGorilla::StartDrumming()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::IDLE, true);
	SetEvent(true);
	CCamera* camera = GetCamera();
	if (camera)
	{
		camera->ChangeState(ECameraState::EVENT);
		camera->SetCameraLength(DORAMING_CAMERA_LENGHTH);
		camera->SetCameraCenterPosition(Vector3(m_Position.x, m_Position.y + ADD_DORAMING_CAMERA_HEIGHT, m_Position.z));
	}

	if(!m_DrummingTimeScheduler.IsEmpty())
	{
		m_DrummingTimeScheduler.Reset();
		return;
	}
	m_DrummingTimeScheduler.Add(3.0f, [=, this]()
	{
		PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::DRUMMING, true);
		PlayVibration(EVibrationType::EXTRA_LARGE);
		PlaySoundIndex(ESoundIndex::DORAMING);
		PlayCameraShake(ECameraShakeType::LOW);
	});
	m_DrummingTimeScheduler.Add(3.16f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::DORAMING);
	});
	m_DrummingTimeScheduler.Add(3.2f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::DORAMING);
	});
	m_DrummingTimeScheduler.Add(3.5f, [=, this]()
	{
		PlayCameraShake(ECameraShakeType::LOW);
		PlaySoundIndex(ESoundIndex::DORAMING);
	});
	m_DrummingTimeScheduler.Add(3.7f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::DORAMING);
	});
	m_DrummingTimeScheduler.Add(4.0f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::DORAMING);
		PlayCameraShake(ECameraShakeType::LOW);
	});
	m_DrummingTimeScheduler.Add(4.2f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::DORAMING);
	});
	m_DrummingTimeScheduler.Add(4.5f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::DORAMING);
		PlayCameraShake(ECameraShakeType::LOW);
	});
	m_DrummingTimeScheduler.Add(4.7f, [=, this]()
	{
		PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::IDLE, true);
	});
	m_DrummingTimeScheduler.Add(5.7f, [=, this]()
	{
		PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::IDLE, true);
		SetEvent(false);
		if (camera)
		{
			camera->ChangeState(ECameraState::NORMAL);
			camera->ResetCameraLength();
		}
	});
}

void CGorilla::UpdateDrumming(float DeltaTime)
{
	m_DrummingTimeScheduler.Run(DeltaTime);
	if (m_DrummingTimeScheduler.IsEndTask())
	{
		m_StateMachine.ChangeState(EGorillaState::IDLE);
	}
}

void CGorilla::StartIdle()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::IDLE, true);
}

void CGorilla::UpdateIdle(float DeltaTime)
{
	if (!m_Player)
	{
		return;
	}

	if (m_StateMachine.GetCurrentStateTime() > ATTACK_START_TIME)
	{
		ChoiceNextAttackState();
		return;
	}
	TowardThePlayer(WAIT_ROTATION_SPEED, DeltaTime);

	if (m_CurrentAnimationNumber == static_cast<int>(EGorillaAnimationNumber::IDLE))
	{
		if (!m_Player->IsGround() || m_Player->IsJumpUp())
		{
			PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::JUMP_GUARD, true);
		}
	}
	else if (m_CurrentAnimationNumber == static_cast<int>(EGorillaAnimationNumber::JUMP_GUARD))
	{
		if (m_Player->IsGround() && !m_Player->IsJumpUp())
		{
			PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::IDLE, true);
		}
	}
}

void CGorilla::StartPlayerMove()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_START, true);
	m_SwingEffectId = Play3DSoundIndex(ESoundIndex::SWING, m_Position);
}

void CGorilla::UpdatePlayerMove(float DeltaTime)
{
	if (!m_Player)
	{
		return;
	}

	if (IsAnimationEnd<EGorillaAnimationNumber>(EGorillaAnimationNumber::ATTACK_START))
	{
		PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_LOOP, true);
	}
	if (IsAnimationEnd<EGorillaAnimationNumber>(EGorillaAnimationNumber::ATTACK_END))
	{
		m_StateMachine.ChangeState(EGorillaState::IDLE);
		return;
	}

	if (m_CurrentAnimationNumber != static_cast<int>(EGorillaAnimationNumber::ATTACK_LOOP))
	{
		return;
	}
	bool isPlayerDamage = false;
	if (CCollison::CollisionCheck(m_Player->GetCapsule(), m_BodyCapsule.get()))
	{
		if (!m_Player->IsDamageState())
		{
			m_Player->Damage(PLAYER_DAMAGE);
			isPlayerDamage = true;
		}
	}

	if (m_SwingEffectId != UINT_MAX)
	{
		Update3DSoundPosition(m_SwingEffectId, m_Position);
	}

	const bool isMove = m_StateMachine.GetCurrentStateTime() < PLAYER_MOVE_TIME && !isPlayerDamage;

	if (isMove)
	{
		SpeedParameter& currentSpeedParameter = m_LifePlayerMoveSpeedList[m_HP.GetCurrentHP()];
		PlayerMove(currentSpeedParameter, DeltaTime);
	}
	//プレイヤーへの追尾終了
	else
	{
		DeleteSoundIndex(ESoundIndex::SWING);
		PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_END, true);
	}
}

void CGorilla::StartJumpAttack()
{
	m_JumpAttackStateMachine.SetInitialState(EJumpAttackState::START);
	m_JumpAttackCount++;
}

void CGorilla::UpdateJumpAttack(float DeltaTime)
{
	m_JumpAttackStateMachine.Update(DeltaTime);
}

void CGorilla::StartJumpAttackStart()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_JUMP_START, true);
}

void CGorilla::UpdateJumpAttackStart(float DeltaTime)
{
	if (IsAnimationEnd<EGorillaAnimationNumber>(EGorillaAnimationNumber::ATTACK_JUMP_START))
	{
		PlaySoundIndex(ESoundIndex::GORILLA_JUMP_LOOP);
		PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_JUMP_LOOP, true);
		m_JumpAttackStateMachine.ChangeState(EJumpAttackState::LOOP);
	}
}

void CGorilla::StartJumpAttackLoop()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_JUMP_LOOP, true);
}

void CGorilla::UpdateJumpAttackLoop(float DeltaTime)
{
	if (m_Position.y < JUMP_ATTACK_HEIGHT)
	{
		m_Position.y += JUMP_ATTACK_UP_SPEED * DeltaTime;
		if (m_Position.y > JUMP_ATTACK_HEIGHT)
		{
			m_Position.y = JUMP_ATTACK_HEIGHT;
		}
	}
	else
	{
		SpeedParameter& currentSpeedParameter = m_LifeJumpAttackSpeedList[m_HP.GetCurrentHP()];
		const bool isTargetGoal = PlayerMove(currentSpeedParameter, DeltaTime);

		if (m_JumpAttackStateMachine.GetCurrentStateTime() > JUMP_ATTACK_MOVE_TIME || isTargetGoal)
		{
			DeleteSoundIndex(ESoundIndex::GORILLA_JUMP_LOOP);
			m_JumpAttackStateMachine.ChangeState(EJumpAttackState::END);
		}
	}
}

void CGorilla::StartJumpAttackEnd()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_JUMP_END, true);
	m_AttackSphere->SetRadius(ATTACK_SPHERE_RADIUS);
}

void CGorilla::UpdateJumpAttackEnd(float DeltaTime)
{
	if (!m_Player)
	{
		return;
	}

	m_Position.y -= JUMP_ATTACK_DOWN_SPEED * DeltaTime;

	SpeedParameter& currentSpeedParameter = m_LifeJumpAttackSpeedList[m_HP.GetCurrentHP()];
	const bool isTargetGoal = PlayerMove(currentSpeedParameter, DeltaTime);
	if (isTargetGoal)
	{
		TowardThePlayer(JUMP_ATTACK_FALL_ROTATION_SPEED, DeltaTime);
	}

	if (m_AttackSphere)
	{
		m_AttackSphere->SetAddSpherePosition(m_Front * ATTACK_SPHERE_FRONT_RATE);
		m_AttackSphere->Update(m_Position);
	}

	if (CCollison::CollisionCheck(m_Player->GetCapsule(), m_AttackSphere.get()))
	{
		m_Player->Damage(PLAYER_DAMAGE);
	}

	RayCastInfo rayCastInfo;
	if (UpdateGroundCollision(DeltaTime, rayCastInfo, false))
	{
		m_Position.y = rayCastInfo.Point.y;
		PlayVibration(EVibrationType::LOW);
		PlayCameraShake(ECameraShakeType::MIDDLE);
		PlayEffect(EEffectType::GROUND_HIT, Vector3(m_Position.x + (m_Front.x * 3.0f), m_Position.y + 2.0f, m_Position.z + (m_Front.z * 3)), 8.5f);
		PlaySoundIndex(ESoundIndex::GORILLA_JUMP_ATTACK);
		m_JumpAttackStateMachine.ChangeState(EJumpAttackState::WAIT);
	}
}

void CGorilla::UpdateJumpAttackWait(float DeltaTime)
{
	if (m_JumpAttackStateMachine.GetCurrentStateTime() < JUMP_ATTACK_WAIT_TIME)
	{
		return;
	}

	if (m_AttackSphere)
	{
		m_AttackSphere->SetRadius(0.0f);
	}

	//指定回数まではジャンプ攻撃を繰り返す
	const int currentHP = m_HP.GetCurrentHP();
	if (!m_LifeJumpAttackList.contains(currentHP))
	{
		return;
	}
	const int lifeJumpAttackCount = m_LifeJumpAttackList[currentHP];
	if (m_JumpAttackCount < lifeJumpAttackCount)
	{
		m_StateMachine.ChangeState(EGorillaState::JUMP_ATTACK, true);
	}
	else
	{
		m_StateMachine.ChangeState(EGorillaState::IDLE);
	}	
}

void CGorilla::StartDeath()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::DEATH, true);
	PlaySoundIndex(ESoundIndex::DOWN2);
	DeleteBGM();

	if (!m_DeathTimeScheduler.IsEmpty())
	{
		m_DeathTimeScheduler.Reset();
		return;
	}
	m_DeathTimeScheduler.Add(0.5f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::DOWN);
	});
	m_DeathTimeScheduler.Add(2.0f, [=, this]()
	{
		std::weak_ptr<CScene> scene = GetScene();
		if (scene.expired())
		{
			return;
		}
		PlaySoundIndex(ESoundIndex::BOSS_DEATH);
		CStar* star = new CStar(Vector3(m_Position.x, m_Position.y + 6.0f, m_Position.z));
		if (star)
		{
			star->SetEmergenceState();
			star->EnableStarSceneChange();
			star->SetOriginalNumber(-1);
		}

		scene.lock()->AddGameObject<CStar>(ESceneObjectType::NOMESH_OBJECT, star);
		PlayEffect(EEffectType::EXPLOSION2, Vector3(m_Position.x + m_Front.x, m_Position.y + 5.0f, m_Position.z + m_Front.z), 11.0f);
		PlayEffect(EEffectType::SMOKE, Vector3(m_Position.x, m_Position.y + 5.0f, m_Position.z), 7.5f);
		m_IsDestroy = true;
	});
}

void CGorilla::UpdateDeath(float DeltaTime)
{
	m_DeathTimeScheduler.Run(DeltaTime);
}

void CGorilla::StartStun()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_HIT_START, true);
}

void CGorilla::UpdateStun(float DeltaTime)
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	if (!IsAnimationEnd<EGorillaAnimationNumber>(EGorillaAnimationNumber::ATTACK_HIT_START))
	{
		return;
	}
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_HIT_LOOP);
	m_StunEffectId = PlayEffect(EEffectType::STUN, Vector3(m_Position.x + (m_Front.x * 2), m_Position.y + 6.25f, m_Position.z + (m_Front.z * 2)), 4.5f);
	PlaySoundIndex(ESoundIndex::STUN);
	m_Arrow = new CArrow(Vector3(m_Position.x + (m_Front.x * 2.0f), m_Position.y + 6.1f, m_Position.z + (m_Front.z * 2.0f)));
	scene.lock()->AddGameObject<CArrow>(ESceneObjectType::NOMESH_OBJECT, m_Arrow);
}

void CGorilla::StartDamage()
{
	PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_HITDAMAGE, true);
	if (!m_DamageTimeScheduler.IsEmpty())
	{
		m_DamageTimeScheduler.Reset();
		return;
	}
	m_DamageTimeScheduler.Add(0.33f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::GORILLA_ATTACK_HEAL);
	});
	m_DamageTimeScheduler.Add(0.75f, [=, this]()
	{
		PlaySoundIndex(ESoundIndex::GORILLA_ATTACK_HEAL);
	});
}

void CGorilla::UpdateDamage(float DeltaTime)
{
	if (IsAnimationEnd<EGorillaAnimationNumber>(EGorillaAnimationNumber::ATTACK_HITDAMAGE))
	{
		if (IsDeath())
		{
			m_StateMachine.ChangeState(EGorillaState::DEATH);
			return;
		}
		PlayAnimation<EGorillaAnimationNumber>(m_AnimationDataList, EGorillaAnimationNumber::ATTACK_HITEND, true);
	}
	if (IsAnimationEnd<EGorillaAnimationNumber>(EGorillaAnimationNumber::ATTACK_HITEND))
	{
		m_StateMachine.ChangeState(EGorillaState::IDLE);
	}

	m_DamageTimeScheduler.Run(DeltaTime);
}
