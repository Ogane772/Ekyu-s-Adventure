#include "Player.h"
#include "State/PlayerStateMachine.h"
#include "State/PlayrStateTypeAndAnimationNumber.h"
#include "../../Object/Door/StarDoor.h"
#include "../../../Framework/Input/InputInterface.h"
#include "../../../Framework/Scene/Scene.h"
#include "../../../Framework/Scene/SceneManager.h"
#include "../../../Framework/Model/StaticMesh.h"
#include "../../../Framework/Collision/Sphere.h"
#include "../../../Framework/Collision/Capsule.h"
#include "../../../Framework/Text/Text.h"
#include "../../../Framework/Collision/Collision.h"
#include "../../../Framework/Collision/CollisionTag.h"
#include "../../../Framework/Camera/Camera.h"
#include "../../../Framework/Fade/Fade.h"
#include "../../../Framework/Sound/SoundTypes.h"
#include "../../../Framework/HitStop/HitStop.h"
#include "../../../Framework/Shadow/ShadowType.h"
#include "../../../Game/Character/Enemy/Enemy.h"
#include "../../../Game/Object/ItemObject/Star.h"
#include "../../../Game/Object/ItemObject/Heart.h"
#include "../../../Game/Effect/EffectTypes.h"

namespace
{
	//最大HP
	constexpr int MAX_LIFE = 5;
	//しっぽアタック攻撃判定開始フレーム
	constexpr float TAIL_ATTACK_START_TIME = 0.18f;
	constexpr float TAIL_ATTACK_END_TIME = 0.5f;
	//無敵時間
	constexpr float INVINCIBLE_TIME = 1.0f;
	//滑る床と判定する角度
	constexpr float SLIP_FLOOR = 40.0f;
	//床滑り時の加速度
	constexpr float SLIP_SPEED = 0.35f;
	//空中移動速度
	constexpr float AIR_MOVESPEED = 0.14f;
	//無敵時間中の点滅を行う際の消滅時間
	constexpr float BLINKINVINCIBLE_INTERVAL = 4.0f / 60.0f;
	//移動時の足音再生時間
	constexpr float RUN_SE_INTERVAL = 0.25f;
	constexpr float WALK_SE_INTERVAL = 0.5f;
	//二段ジャンプ、3段ジャンプ等の次のジャンプまでの猶予フレーム
	constexpr float NEXT_JUMP_TRANSITION_TIME = 0.16f;
	//天井判定に使用する線へ足す高さの開始/終了
	constexpr float START_UPLINE_HEIGHT = 2.0f;
	constexpr float END_UPLINE_HEIGHT = 3.0f;
	//崖判定の線へ足す高さ
	constexpr float ADD_CLIFFLINE_HEIGHT = 2.0f;
	//崖判定の上から飛ばす線へ足す高さ
	constexpr float ADD_CLIFF_UPLINE_HEIGHT = 0.5f;
	//崖判定とみなす内積の値
	constexpr float CLIFF_HIT_DOT = 0.85f;
	//崖上がり時にゴール位置に足す高さ
	constexpr float ADD_GOAL_CLIFFUP_HEIGHT = 2.4f;
	//崖上がりの移動時間
	constexpr float CLIFFUP_MOVETIME = 0.58f;
	//崖上がりの移動補間速度
	constexpr float CLIFFUP_Y_MOVETIME = 0.09f;
	constexpr float CLIFFUP_XZ_MOVETIME = 0.06f;
	//壁滑り判定の線へ足す高さ
	constexpr float ADD_WALLSILIPLINE_HEIGHT = 2.0f;
	//壁滑り判定の移動方向に入力しているとみなす内積の値
	constexpr float WALLSLIP_INPUT_DOT = 0.9f;
	//壁滑り判定の壁に向いているとみなす内積の値
	constexpr float WALLSLIP_WALL_ANGLE_DOT = -0.6f;
	// 基本重力値
	constexpr float GRAVITY_POWER = 3.0f;
	// 汎用的な重力値
	constexpr float COMMON_GRAVITY_POWER = (GRAVITY_POWER - (GRAVITY_POWER * 0.5f));
	// ヒップドロップやダイブジャンプ等の加速度が高い重力値
	constexpr float POWER_GRAVITY_POWER = GRAVITY_POWER * 2.0f;
	//下降中の最大速度
	constexpr float MAX_DOWN_SPEED = -0.67f;
	//ヒップドロップ下降中の最大速度
	constexpr float MAX_HIPDROP_DOWN_SPEED = -1.5f;
	//着地時の地面にプレイヤーの高さを合わせる速度
	constexpr float LANDING_HEIGHT_SPEED = 18.75f;
	//スピンジャンプの上昇時間
	constexpr float SPINJUMP_MOVETIME = 0.35f;
	//スピンジャンプの上昇速度
	constexpr float SPINJUMP_MOVESPEED = 4.8f;
	//反転ジャンプの移動時間
	constexpr float TURN_JUMP_MOVETIME = 0.75f;
	//足元のラインの開始位置に足す高さ
	constexpr float ADD_DOWNLINE_START_LENGTH = 1.5f;
	//足元のラインの長さ
	constexpr float DOWNLINE_LENGTH = 0.6f;
	//ダッシュジャンプの力
	constexpr float DASH_JUMP_POWER = 0.46f;
	//ヒップドロップからの強化ジャンプ
	constexpr float HIGH_JUMP_POWER = 0.57f;
	//振り向きジャンプの力
	constexpr float TURN_JUMP_POWER = 0.66f;
	//しゃがみジャンプの力
	constexpr float CROUCHING_JUMP_POWER = 0.62f;
	//飛び込みジャンプの力
	constexpr float DIVE_JUMP_POWER = 0.56f;
	//壁ジャンプの力
	constexpr float WALL_JUMP_POWER = 0.5f;
	//ジャンプの力
	constexpr float NORMAL_JUMP_POWER = 0.47f;
	//二段ジャンプの力
	constexpr float SECOND_JUMP_POWER = 0.56f;
	//三段ジャンプの力
	constexpr float FINAL_JUMP_POWER = 0.7f;
	//地上移動において、走りと歩きアニメーションを切り替える基準となる最高速度から引くマージン
	constexpr float LANDMOVE_RUN_SPEED_MARGIN = 0.05f;
	//入力を伴う前移動において、前移動以外の入力として無視する値
	constexpr float IGNORE_FRONT_INPUT = 0.85f;
	//三段ジャンプに必要な移動速度
	constexpr float REQUIRED_FINALJUMP_MOVE_SPEED = 0.14f;
	//壁ヒットとして扱うDotの値
	constexpr float WALL_HIT_DOT = 0.7f;
	//壁滑り時の速度
	constexpr float WALL_SLIP_SPEED = 13.0f;
	//振り向き入力がされたと判定する角度開始/終了の値
	constexpr float TURN_START_ROTATION = 120.0f;
	constexpr float TURN_END_ROTATION = 270.0f;
	//ドングリを100個集めたときの出現スター位置の高さ
	constexpr float ADD_STAR_SPAWN_HEIGHT = 5.0f;
	//スター取得時のスターを出す高さに足す値
	constexpr float ADD_GETSTAR_HEIGHT = 3.3f;
	//ステージクリアスター取得時の待機時間
	constexpr float WAIT_STAR_STAGECLEAR_TIME = 2.1f;
	//着地時のエフェクトに足す高さと大きさ
	constexpr float ADD_LANDING_EFFECT_HEIGHT = 0.2f;
	constexpr float LANDING_EFFECT_SIZE = 2.5f;
	//壁ヒット時のエフェクトの大きさ
	constexpr float WALLHIT_EFFECT_SIZE = 4.5f;
	//ダイブジャンプのエフェクトに足す高さと大きさ
	constexpr float ADD_DIVEJUMP_EFFECT_HEIGHT = 1.0f;
	constexpr float DIVEJUMP_EFFECT_SIZE = 2.5f;
	//ダメージのエフェクトに足す高さと大きさ
	constexpr float ADD_DAMAGE_EFFECT_HEIGHT = 1.5f;
	constexpr float DAMAGE_EFFECT_SIZE = 4.5f;
	//回復のエフェクトに足す高さと大きさ
	constexpr float ADD_HEART_EFFECT_HEIGHT = 2.3f;
	constexpr float HEART_EFFECT_SIZE = 3.8f;
	//ダイブジャンプの攻撃コリジョンの大きさと足す高さ
	constexpr float DIVEJUMP_ATTACK_COLLISION_SIZE = 1.3f;
	constexpr float DIVEJUMP_ATTACK_COLLISION_HEIGHT = 0.6f;
	//しっぽアタックの攻撃コリジョンの大きさと足す高さ
	constexpr float TAILATTACK_ATTACK_COLLISION_SIZE = 1.6f;
	constexpr float TAILATTACK_ATTACK_COLLISION_HEIGHT = 1.5f;
	//落下中の攻撃コリジョンの大きさと足す高さ
	constexpr float FALL_ATTACK_COLLISION_SIZE = 0.8f;
	constexpr float FALL_ATTACK_COLLISION_HEIGHT = -0.2f;
	//スターを出現させる取得ドングリ数
	constexpr int STAR_ACORN_COUNT = 100;
};

CPlayer::CPlayer() : m_StateMachine(std::make_unique<CPlayerStateMachine>(this))
{
	m_AnimationDataList =
	{
		{ EPlayerAnimationNumber::IDLE,			   AnimationData(0.47f, true)  },
		{ EPlayerAnimationNumber::IDLE_PINCH,	       AnimationData(0.52f, true)  },
		{ EPlayerAnimationNumber::WALK,			   AnimationData(0.4f,  true)  },
		{ EPlayerAnimationNumber::RUN,			   AnimationData(0.7f,  true)  },
		{ EPlayerAnimationNumber::CROUCHING,         AnimationData(0.4f,  true)  },
		{ EPlayerAnimationNumber::CROUCHING_WALK,    AnimationData(0.42f, true)  },
		{ EPlayerAnimationNumber::CROUCHING_JUMP,    AnimationData(0.92f, false) },
		{ EPlayerAnimationNumber::MOVE_CHANGE,	   AnimationData(0.52f, false) },
		{ EPlayerAnimationNumber::MOVE_CHANGE_JUMP,  AnimationData(0.72f, false) },
		{ EPlayerAnimationNumber::DASH_JUMP,		   AnimationData(0.92f, false) },
		{ EPlayerAnimationNumber::JUMP_START,	       AnimationData(0.52f, false) },
		{ EPlayerAnimationNumber::JUMP_LOOP,		   AnimationData(0.62f, false) },
		{ EPlayerAnimationNumber::SECOND_JUMP,	   AnimationData(1.4f,  false) },
		{ EPlayerAnimationNumber::FINAL_JUMP_START,  AnimationData(1.4f,  false) },
		{ EPlayerAnimationNumber::FINAL_JUMP_LOOP,   AnimationData(1.4f,  true)  },
		{ EPlayerAnimationNumber::CLIFF_START,       AnimationData(0.52f, false) },
		{ EPlayerAnimationNumber::CLIFF_LOOP,		   AnimationData(0.4f,  true)  },
		{ EPlayerAnimationNumber::CLIFF_END,		   AnimationData(0.62f, false) },
		{ EPlayerAnimationNumber::SPIN_JUMP,		   AnimationData(2.0f,  false) },
		{ EPlayerAnimationNumber::WALL_SLIP,		   AnimationData(0.52f, true)  },
		{ EPlayerAnimationNumber::DIVE_JUMP,		   AnimationData(1.0f,  false) },
		{ EPlayerAnimationNumber::WALL_HIT_START,    AnimationData(0.52f, false) },
		{ EPlayerAnimationNumber::WALL_HIT_SIT,      AnimationData(0.4f,  false) },
		{ EPlayerAnimationNumber::WALL_HIT_END,      AnimationData(0.4f,  false) },
		{ EPlayerAnimationNumber::HIPDROP,		   AnimationData(1.0f,  false) },
		{ EPlayerAnimationNumber::HIPDROP_END,	   AnimationData(0.62f, false) },
		{ EPlayerAnimationNumber::HIGH_JUMP,		   AnimationData(1.2f,  false) },
		{ EPlayerAnimationNumber::FALL,			   AnimationData(1.0f,  true)  },
		{ EPlayerAnimationNumber::TAIL_ATTACK,	   AnimationData(0.9f,  false) },
		{ EPlayerAnimationNumber::GROUND_SLIP,	   AnimationData(0.52f, false) },
		{ EPlayerAnimationNumber::SLIP_DIVE,		   AnimationData(0.52f, false) },
		{ EPlayerAnimationNumber::GET_STAR,		   AnimationData(0.5f,  false) },
		{ EPlayerAnimationNumber::DAMAGE,		       AnimationData(0.4f,  false) },
		{ EPlayerAnimationNumber::DEATH,			   AnimationData(0.5f,  false) },
	};

	m_SpeedParameterList =
	{
		{ EPlayerSpeedParameterType::LAND_MOVE, SpeedParameter(0.03f, 0.19f, 8.0f, 20.0f, 15.0f) },
		{ EPlayerSpeedParameterType::LAND_TURN, SpeedParameter(0.0f, 0.0f, 0.0f, 6.5f, 0.0f) },
		{ EPlayerSpeedParameterType::CROUCHING_MOVE, SpeedParameter(0.01f, 0.09f, 5.0f, 1.0f, 10.0f) },
		{ EPlayerSpeedParameterType::CROUCHING_SLIDE, SpeedParameter(0.0f, 0.0f, 0.0f, 3.5f, 3.5f) },
		{ EPlayerSpeedParameterType::CROUCHING_JUMP, SpeedParameter(0.0f, 0.13f, 2.0f, 3.5f, 0.0f) },
		{ EPlayerSpeedParameterType::TAIL_ATTACK, SpeedParameter(0.0f, 0.14f, 1.0f, 2.0f, 0.0f) },
		{ EPlayerSpeedParameterType::AIR_MOVE, SpeedParameter(0.0f, 0.14f, 10.0f, 1.0f, 5.0f) },
		{ EPlayerSpeedParameterType::SPIN_JUMP, SpeedParameter(0.0f, 0.1f, 10.0f, 5.0f, 0.0f) },
		{ EPlayerSpeedParameterType::DIVE_JUMP, SpeedParameter(0.0f, 0.30f, 20.0f, 2.0f, 0.0f) },
		{ EPlayerSpeedParameterType::WALL_JUMP, SpeedParameter(0.01f, 0.27f, 10.0f, 15.0f, 0.0f) },
		{ EPlayerSpeedParameterType::DASH_JUMP, SpeedParameter(0.01f, 0.5f, 12.0f, 15.0f, 0.0f) },
		{ EPlayerSpeedParameterType::TURN_JUMP, SpeedParameter(0.0f, 0.15f, 3.0f, 3.0f, 0.0f) },
		{ EPlayerSpeedParameterType::WALL_HIT_MOVE, SpeedParameter(0.0f, 0.0f, 0.0f, 10.0f, 0.0f) },
	};
}

void CPlayer::Init()
{
	m_IsNoCullingCheck = true;
	m_HP.SetMaxHP(MAX_LIFE);
	m_HP.SetHP(MAX_LIFE);
	m_SkeletalMesh = std::make_unique<CSkeletalMesh>(ESkeletalMeshType::PLAYER);
	m_StarModel = std::make_unique<CStaticMesh>(EStaticMeshType::STAR);
	m_DefaultPosition = m_Position;
	m_Scale = Vector3(8.0f, 8.0f, 8.0f);
	CreateVector();
	m_DownLine = std::make_unique<CLine>(&m_Position, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, -DOWNLINE_LENGTH, 0.0f));
	m_FrontLine = std::make_unique<CLine>(&m_Position, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 1.0f));
	m_CliffLine = std::make_unique<CLine>(&m_Position, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 1.0f));
	m_UpLine = std::make_unique<CLine>(&m_Position, Vector3(0.0f, 2.0f, 0.0f), Vector3(0.0f, 3.0f, 0.0f));
	m_Capsule = std::make_unique<CCapsule>(Vector3(0, 0.9f,0), Vector3(0, 2.5f, 0.0), Vector3(0.0, 0.0f, 0.1f), 0.6f, 8, 8);
	m_AttackSphere = std::make_unique<CSphere>(m_Position, 0.0f);
	if (m_StateMachine)
	{
		m_StateMachine->ChangeState(EPlayerStateType::LAND_IDLE);
	}
}

void CPlayer::UnInit()
{
	m_SkeletalMesh.reset();
	m_StarModel.reset();
	m_Capsule.reset();
	m_AttackSphere.reset();
	m_DownLine.reset();
	m_FrontLine.reset();
	m_CliffLine.reset();
	m_UpLine.reset();
	m_StateMachine.reset();
}

void CPlayer::Update(float DeltaTime)
{
	if (IsEvent())
	{
		if (m_SkeletalMesh)
		{
			m_SkeletalMesh->Update(DeltaTime);
		}
		return;
	}

	if (IsEditMode())
	{
		UpdateWorldMatrix();
		return;
	}
	if (CFade* fade = CFade::GetInstance())
	{
		if (fade->IsFade())
		{
			if (m_SkeletalMesh)
			{
				m_SkeletalMesh->Update(DeltaTime);
			}
			UpdateWorldMatrix();
			return;
		}
	}

	if (m_StateMachine)
	{
		m_StateMachine->Update(DeltaTime);
	}

	if (m_SkeletalMesh)
	{
		m_SkeletalMesh->Update(DeltaTime);
	}

	UpdateInvincibleCount(DeltaTime);

	if (m_Capsule)
	{
		m_Capsule->Update(m_Position);
	}
	//カプセルと壁の判定
	if (m_EnableCollision)
	{
		CheckBodyCollision();

		//高さは押し出されないように高さを保存する
		float savePositionY = m_Position.y;
		HitResult hitResult(&m_Position, &m_WallNormal);
		m_IsCapsuleHit = CCollison::Capsule_VS_Mesh(ECheckCollisionType::PLAYER, m_Capsule.get(), hitResult);
		m_Position.y = savePositionY;
		if (m_Capsule)
		{
			m_Capsule->Update(m_Position);
		}
	}

	UpdateWorldMatrix();
}

void CPlayer::Draw()
{
	if (m_IsEditorDisableShadow)
	{
		m_SkeletalMesh->Draw(m_World);
	}
	else if (!IsBlinkInvincible())
	{
		m_SkeletalMesh->ShadowOnModelDraw(EShadowType::DOWN_SHADOW, m_World);
	}

	DrawGetStar();

	if (m_DownLine)
	{
		m_DownLine->Draw();
	}
	if (m_FrontLine)
	{
		m_FrontLine->Draw();
	}
	if (m_CliffLine)
	{
		m_CliffLine->Draw();
	}
	if (m_UpLine)
	{
		m_UpLine->Draw();
	}
	if (m_Capsule)
	{
		m_Capsule->Draw();
	}
	if (m_AttackSphere)
	{
		m_AttackSphere->Draw();
	}
}

void CPlayer::ShadowDraw()
{
	if (!m_IsEditorDisableShadow)
	{
		if (m_SkeletalMesh)
		{
			m_SkeletalMesh->ShadowDraw(EShadowType::DOWN_SHADOW, m_World);
		}
	}
	DrawGetStar();
}

void CPlayer::Damage(const int Damage)
{
	if (CFade* fade = CFade::GetInstance())
	{
		if (fade->IsFade())
		{
			return;
		}
	}
	if (m_IsDebugNoDamage)
	{
		return;
	}

	if (IsDamageState())
	{
		return;
	}

	if (m_IsInvincible)
	{
		return;
	}
	
	PlayHitStop(EHitStopType::MIDDLE);
	PlayEffect(EEffectType::PLAYER_HIT, Vector3(m_Position.x, m_Position.y + ADD_DAMAGE_EFFECT_HEIGHT, m_Position.z), DAMAGE_EFFECT_SIZE);;
	PlaySoundIndex(ESoundIndex::PLAYER_DAMAGE);
	m_HP.AddDamage(Damage);
	PlayVibration(EVibrationType::LOW);
	if (m_StateMachine)
	{
		m_StateMachine->ChangeState(EPlayerStateType::COMMON_DAMAGE);
	}
}

void CPlayer::StartInvincible()
{
	m_IsInvincible = true;
	m_InvincibleCountTime = 0.0f;
}

void CPlayer::UpdateInvincibleCount(const float DeltaTime)
{
	if (!m_IsInvincible)
	{
		return;
	}
	if (m_InvincibleCountTime > INVINCIBLE_TIME)
	{
		m_IsInvincible = false;
		m_InvincibleCountTime = 0.0f;
		return;
	}
	m_InvincibleCountTime += DeltaTime;
}

bool CPlayer::IsBlinkInvincible() const
{
	if (!m_IsInvincible)
	{
		return false;
	}
	const int blinkCount = static_cast<int>(m_InvincibleCountTime / BLINKINVINCIBLE_INTERVAL) % 2;
	return blinkCount == 0;
}

bool CPlayer::IsDamageOrDeath() const
{
	return IsDamageState() || IsDeath() || m_IsInvincible;
}

bool CPlayer::IsDamageState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::COMMON_DAMAGE;
}

bool CPlayer::IsCongratulationState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::COMMON_CONGRATULATION;
}

bool CPlayer::IsHipDropState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::AIR_HIPDROP;
}

bool CPlayer::IsDiveJumpState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::AIR_DIVE_JUMP;
}

bool CPlayer::IsTailAttackState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::LAND_TAIL_ATTACK;
}

bool CPlayer::IsEnemyKnockBackDamageState() const
{
	return IsDiveJumpState() || IsTailAttackState();
}

bool CPlayer::IsEnemyFallDamageState() const
{
	return IsFall() || IsHipDropState();
}

bool CPlayer::IsCliffState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::AIR_CLIFF;
}

bool CPlayer::IsNotInputState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	const EPlayerStateType type = m_StateMachine->GetCurrentState();
	return IsDamageState() ||
		type == EPlayerStateType::COMMON_DEATH ||
		type == EPlayerStateType::AIR_HIPDROP ||
		type == EPlayerStateType::COMMON_WALL_HIT ||
		type == EPlayerStateType::LAND_DASH_JUMP ||
		type == EPlayerStateType::LAND_TAIL_ATTACK;
}

bool CPlayer::IsEnemyFallJumpState() const
{
	return IsFall() && !IsHipDropState();
}

bool CPlayer::IsNormalJumpableState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return IsCliffState() || 
		m_LandedState == ELandedState::LAND &&
		m_StateMachine->GetCurrentState() != EPlayerStateType::LAND_CROUCHING_SLIDE &&
		!IsCrouchingJumpableState() &&
		!IsTurnJumpableState() &&
		!IsNotInputState();
}

bool CPlayer::IsSpinJumpableState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	if (m_IsPlaySpinJumpAction || IsNotInputState() || IsCliffState())
	{
		return false;
	}
	const EPlayerStateType type = m_StateMachine->GetCurrentState();
	return m_LandedState == ELandedState::AIR &&
		type != EPlayerStateType::AIR_DIVE_JUMP;
}

bool CPlayer::IsDiveJumpableState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	if (IsNotInputState() || IsCliffState())
	{
		return false;
	}
	return m_LandedState == ELandedState::AIR
		&& m_StateMachine->GetCurrentState() != EPlayerStateType::AIR_DIVE_JUMP;
}

bool CPlayer::IsHighJumpableState() const
{
	return m_CurrentAnimationNumber == static_cast<int>(EPlayerAnimationNumber::HIPDROP_END);
}

bool CPlayer::IsWallJumpableState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::AIR_WALL_SLIP;
}

bool CPlayer::IsTurnJumpableState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::LAND_TURN;
}

bool CPlayer::IsDashJumpableState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::LAND_CROUCHING_SLIDE;
}

bool CPlayer::IsTailAttackPossibleState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	if (IsNotInputState())
	{
		return false;
	}
	const EPlayerStateType type = m_StateMachine->GetCurrentState();
	return m_LandedState == ELandedState::LAND &&
		type != EPlayerStateType::LAND_TAIL_ATTACK &&
		type != EPlayerStateType::LAND_SLOPE_SLIP;
}

bool CPlayer::IsCrouchingPossibleState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	if (IsNotInputState())
	{
		return false;
	}
	const EPlayerStateType type = m_StateMachine->GetCurrentState();
	return type == EPlayerStateType::LAND_IDLE ||
		   type == EPlayerStateType::LAND_MOVE;
}

bool CPlayer::IsCrouchingJumpableState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	const EPlayerStateType type = m_StateMachine->GetCurrentState();
	return type == EPlayerStateType::LAND_CROUCHING_IDLE ||
		type == EPlayerStateType::LAND_CROUCHING_MOVE;
}

bool CPlayer::IsHipDropPossibleState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	if (IsNotInputState() || IsCliffState())
	{
		return false;
	}
	return m_LandedState == ELandedState::AIR &&
		m_StateMachine->GetCurrentState() != EPlayerStateType::AIR_DIVE_JUMP;
}

void CPlayer::StartDeath()
{
	StopBGM();
	PlaySoundIndex(ESoundIndex::DEATH);
	TurnTowardsCamera();
	if (CCamera* camera = GetCamera())
	{
		camera->ChangeState(ECameraState::DEATH);
	}
}

void CPlayer::EndDeath()
{
	CSceneManager* manager = CSceneManager::GetInstance();
	if (manager)
	{
		manager->RetryScene();
	}
}

void CPlayer::StartStarDoorAnimation()
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	CStarDoor* starDoor = scene.lock()->GetGameObject<CStarDoor>(ESceneObjectType::MESH_OBJECT);
	if (!starDoor)
	{
		return;
	}
	if (starDoor->IsStartStarDoorAnimation(m_MyStarCount))
	{
		starDoor->StartStarDoorAnimation();
	}
}

bool CPlayer::IsSceneChangeGetStageClearStar() const
{
	if (!m_StateMachine)
	{
		return false;
	}

	if (!m_IsGetStageClearStar)
	{
		return false;
	}

	return  m_StateMachine->GetCurrentState() == EPlayerStateType::COMMON_CONGRATULATION &&
			m_StateMachine->GetCountStateTime() >= WAIT_STAR_STAGECLEAR_TIME;
}

void CPlayer::ChangeFinalJumpLoop()
{
	if (m_CurrentAnimationNumber == static_cast<int>(EPlayerAnimationNumber::FINAL_JUMP_LOOP))
	{
		return;
	}
	PlayAnimationPlayer(EPlayerAnimationNumber::FINAL_JUMP_LOOP, false);
	SetNotBlendAnimation(true);
	PlaySoundIndex(ESoundIndex::FINAL_JUMP);
}

void CPlayer::AddHp(const int Add)
{
	m_HP.RecoverDamage(Add);
	PlayEffect(EEffectType::HEART, Vector3(m_Position.x, m_Position.y + ADD_HEART_EFFECT_HEIGHT, m_Position.z), HEART_EFFECT_SIZE);
}

Vector2 CPlayer::GetInputDirection() const
{
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return Vector2();
	}
	CCamera* camera = GetCamera();
	if (!camera)
	{
		return Vector2();
	}
	const Vector3 cameraFrontVector = camera->GetFront();
	Vector2 inputDirection = instance->GetLeftStick();

	//ゲームパッドの入力をカメラの向きに変換する
	const float inputRotation = (atan2(cameraFrontVector.x, cameraFrontVector.z));
	const XMMATRIX mtxR = DirectX::XMMatrixRotationY(inputRotation);
	//Y軸を曲げるのでZに代入
	Vector3 gamePadMove = Vector3(inputDirection.x, 0.0f, inputDirection.y);
	gamePadMove = Vector3::TransformCoord(gamePadMove, mtxR);
	inputDirection = Vector2(gamePadMove.x, gamePadMove.z);
	return inputDirection;
}

void CPlayer::CheckBodyCollision()
{
	if (IsDeath())
	{
		return;
	}
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	std::vector<CEnemy*> enemys;
	scene.lock()->GetGameEnemyObjects(ESceneObjectType::ENEMY, enemys);

	for (CEnemy* enemy : enemys)
	{
		if (enemy->IsRespawnWait())continue;
		enemy->PlayerHitCheck();
		//エネミーなどの体に当たったら押し出す
		if (enemy->GetHP().GetCurrentHP() == 0)continue;
		float timeY = m_Position.y;
		if (enemy->GetSphere())
		{	
			if (CCollison::CollisionCheck(m_Capsule.get(), enemy->GetSphere(), &m_Position))
			{
				m_Position.y = timeY;
				m_Capsule->Update(m_Position);
			}
		}
		else if (enemy->GetCapsule())
		{
			if (CCollison::CollisionCheck(m_Capsule.get(), enemy->GetCapsule(), &m_Position))
			{
				m_Position.y = timeY;
				m_Capsule->Update(m_Position);
			}
		}
	}
}

void CPlayer::PlayAnimationPlayer(const EPlayerAnimationNumber AnimationNumber, const bool IsBlend)
{
	PlayAnimation<EPlayerAnimationNumber>(m_AnimationDataList, AnimationNumber, IsBlend);
}

void CPlayer::AddMyAcorn()
{
	m_MyAcornCount++;
	//指定数ドングリを取得したらスターをsちゅつげんさせる
	if (m_MyAcornCount == STAR_ACORN_COUNT)
	{
		DestroyFinalJumpSE();
		CStar* star = new CStar(Vector3(m_Position.x,m_Position.y + ADD_STAR_SPAWN_HEIGHT, m_Position.z));
		if (!star)
		{
			return;
		}
		star->SetEmergenceState();
		star->SetOriginalNumber(-1);
		std::weak_ptr<CScene> scene = GetScene();
		if (!scene.expired())
		{
			scene.lock()->AddGameObject<CStar>(ESceneObjectType::NOMESH_OBJECT, star);
		}
	}
}

void CPlayer::SetDefaultPosition(const Vector3& SetPos)
{
	m_DefaultPosition = SetPos;
	if (CCamera* camera =GetCamera())
	{
		camera->SetCameraPosition(m_DefaultPosition);
	}
}

void CPlayer::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_Capsule)
	{
		m_Capsule->Update(m_Position);
	}
	if (m_AttackSphere)
	{
		m_AttackSphere->Update(m_Position);
	}
}

bool CPlayer::IsInputMove() const
{
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return false;
	}
	bool isInputMove = true;
	const Vector2 input = instance->GetLeftStick();
	if(input.LengthSq() == 0.0f)
	{
		isInputMove = false;
	}
	return isInputMove;
}

void CPlayer::InputMove(const EPlayerSpeedParameterType Type, const float DeltaTime)
{
	if (!m_SpeedParameterList.contains(Type))
	{
		return;
	}
	Vector2 inputDirection = GetInputDirection();
	bool isBreak = false;
	//無操作の場合は前方向移動と減速を行う
	if (inputDirection.LengthSq() == 0.0f)
	{
		inputDirection = Vector2(m_Front.x, m_Front.z);
		isBreak = true;
	}
	SpeedParameter& speedParam = m_SpeedParameterList[Type];
	speedParam.MoveDirection = inputDirection;
	Move(speedParam, DeltaTime, isBreak);
}

void CPlayer::FrontMove(const EPlayerSpeedParameterType Type, const float DeltaTime, const bool IsReverseFront /*= false*/)
{
	if (!m_SpeedParameterList.contains(Type))
	{
		return;
	}
	SpeedParameter& speedParam = m_SpeedParameterList[Type];
	speedParam.MoveDirection = m_Front;
	if (IsReverseFront)
	{
		speedParam.MoveDirection *= -1.0f;
	}
	Move(speedParam, DeltaTime);
}

void CPlayer::FrontInputMove(const EPlayerSpeedParameterType Type, const float DeltaTime, const bool IsBreakMoveInput /*= false*/)
{
	if (!m_SpeedParameterList.contains(Type))
	{
		return;
	}
	Vector2 inputDirection = GetInputDirection();

	const float dot = Vector3::Dot(Vector3(inputDirection.x, 0.0f, inputDirection.y), m_Front);
	const bool isMinusInputDirection = dot < 0.0f;
	if (isMinusInputDirection)
	{
		inputDirection *= -1.0f;//符号を正面方向へ反転
	}
	//横方向及び後ろへの入力は前方向として扱う
	if (dot >= -1.0f && dot < IGNORE_FRONT_INPUT)
	{
		//ただし進行方向へ入力された場合はより進行方向へ進みたいため移動量を減らす
		const float moveRate = 0.85f;
		inputDirection = Vector2(m_Front.x, m_Front.z) * moveRate;
	}
	
	bool isForcedBrake = false;
	//ブレーキが有効な時は逆方向へ入力されたら速度を落とす
	if (IsBreakMoveInput && isMinusInputDirection)
	{
		isForcedBrake = true;
	}
	SpeedParameter& speedParam = m_SpeedParameterList[Type];
	speedParam.MoveDirection = inputDirection;
	Move(speedParam, DeltaTime, isForcedBrake);
}

ELandedState CPlayer::CheckGround()
{
	if (!m_DownLine)
	{
		return ELandedState::NONE;
	}
	const bool isLineHit = m_DownLine->LineHitCheck(ECheckCollisionType::PLAYER, Vector3(m_Position.x, m_Position.y + ADD_DOWNLINE_START_LENGTH, m_Position.z), Vector3(m_Position.x, m_Position.y - DOWNLINE_LENGTH, m_Position.z), &m_DownCastInfo);
	if (isLineHit)
	{
		if (m_LandedState == ELandedState::AIR)
		{
			m_LandedState = ELandedState::ON_LANDED;
		}
	}
	else
	{
		m_LandedState = ELandedState::AIR;
	}
	return m_LandedState;
}

void CPlayer::CommonFallMove(float DeltaTime)
{
	if (m_JumpPower > MAX_DOWN_SPEED)
	{
		m_JumpPower -= COMMON_GRAVITY_POWER * DeltaTime;
	}
}

void CPlayer::FallMove(float DeltaTime)
{
	m_Position.y += m_JumpPower * GetTimeScale();
	CommonFallMove(DeltaTime);
	UpdateFallAttackSphere();
}

void CPlayer::OnLanding()
{
	if (!m_StateMachine)
	{
		return;
	}
	
	if (m_StateMachine->GetCurrentState() == EPlayerStateType::AIR_HIPDROP)
	{
		PlayVibration(EVibrationType::MIDDLE);
		PlayCameraShake(ECameraShakeType::MIDDLE);
		PlaySoundIndex(ESoundIndex::HIPDROP_END);
	}
	else
	{
		PlayEffect(EEffectType::JUMP, Vector3(m_DownCastInfo.Point.x, m_DownCastInfo.Point.y + ADD_LANDING_EFFECT_HEIGHT, m_DownCastInfo.Point.z), LANDING_EFFECT_SIZE);
		//暗転中に足音を鳴らさないようにする
		bool isFade = false;
		if (CFade* fade = CFade::GetInstance())
		{
			isFade = fade->IsFade();
		}
		if (!isFade)
		{
			PlaySoundIndex(ESoundIndex::JUMP_END);
		}
		DestroyFinalJumpSE();
	}
	if (!IsSlopeSlip())
	{
		m_SlipGravity = Vector3::Zero();
	}
	ResetJumpPower();
	DisableAttackSphere();
	m_NextJumpTransitionTime = 0.0f;
	m_IsPlaySpinJumpAction = false;
	m_LandedState = ELandedState::LAND;
}

void CPlayer::UpdateLanding(float DeltaTime)
{
	m_Position.y = std::lerp(m_Position.y, m_DownCastInfo.Point.y, LANDING_HEIGHT_SPEED * DeltaTime);
	if (m_NextJumpTransitionTime > NEXT_JUMP_TRANSITION_TIME)
	{
		m_JumpState = ENormalJumpState::NO_JUMP;
	}
	m_NextJumpTransitionTime += DeltaTime;
}

bool CPlayer::IsJumpTrigger() const
{
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return false;
	}
	return instance->IsJumpTrigger();
}

void CPlayer::UpdateJumpFall(float DeltaTime)
{
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return;
	}
	// 落下量調整 下降中
	if (m_JumpPower > 0.0f)
	{
		// 上昇中かつキーが押されている場合は通常落下速度で計算し下降量を減らす
		if (instance->IsJumpPress() && m_JumpPower > 0)
		{
			if (m_JumpPower > MAX_DOWN_SPEED)
			{
				CommonFallMove(DeltaTime);
			}
		}
		else
		{
			m_JumpPower -= GRAVITY_POWER * DeltaTime;
		}
	}
	else if (m_JumpPower > MAX_DOWN_SPEED)
	{
		CommonFallMove(DeltaTime);
	}
	m_Position.y += m_JumpPower * GetTimeScale();
	UpdateFallAttackSphere();
}

bool CPlayer::IsUpWallHit() const
{
	if (!m_UpLine)
	{
		return false;
	}
	RayCastInfo upCastInfo;
	return  m_UpLine->LineHitCheck(ECheckCollisionType::PLAYER, Vector3(m_Position.x, m_Position.y + START_UPLINE_HEIGHT, m_Position.z), Vector3(m_Position.x, m_Position.y + END_UPLINE_HEIGHT, m_Position.z), &upCastInfo);
}

bool CPlayer::IsCliffHit()
{
	if (!m_FrontLine || !m_CliffLine)
	{
		return false;
	}
	if (m_DisableCliffState)
	{
		return false;
	}
	if (IsDamageOrDeath())
	{
		return false;
	}
	Vector3 cliffFront = m_Front;
	//振り向きジャンプ時は逆方向に線を飛ばす
	if (IsTurnJumpState())
	{
		cliffFront *= -1.0f;
	}
	RayCastInfo frontCastInfo;
	const Vector3 lineCliffFront = Vector3(cliffFront.x, cliffFront.y + ADD_CLIFFLINE_HEIGHT, cliffFront.z);
	bool isLineHit = m_FrontLine->LineHitCheck(ECheckCollisionType::PLAYER, Vector3(m_Position.x, m_Position.y + ADD_CLIFFLINE_HEIGHT, m_Position.z), m_Position + lineCliffFront, &frontCastInfo);
	if (!isLineHit)
	{
		return false;
	}

	//対象のオブジェクトが崖掴み無効ならreturn
	if (frontCastInfo.CollisionTag == ECollisionTagName::NO_CLIFF_HIT)
	{
		return false;
	}

	Vector3 cliff = lineCliffFront;
	//上から確認する線なのでY方向に+する
	cliff.y = ADD_CLIFFLINE_HEIGHT + ADD_CLIFF_UPLINE_HEIGHT;

	isLineHit = m_CliffLine->LineHitCheck(ECheckCollisionType::PLAYER, Vector3(m_Position.x + cliff.x, m_Position.y + ADD_CLIFFLINE_HEIGHT, m_Position.z + cliff.z), m_Position + cliff, &m_CliffCastInfo);
	if (!isLineHit)
	{
		return false;
	}

	//壁に対して向いていたらつかまる
	const float dot = fabs(Vector3::Dot(frontCastInfo.Normal, cliffFront));
	if (dot >= CLIFF_HIT_DOT)
	{
		return true;
	}
	
	return false;
}

void CPlayer::StartCliff()
{
	CCamera* camera = GetCamera();
	if (!camera)
	{
		return;
	}
	DestroyFinalJumpSE();
	m_JumpState = ENormalJumpState::NO_JUMP;
	PlaySoundIndex(ESoundIndex::CLIFF_HIT);
	SetRotationYaw(GetAtan2Rotation(Vector2(m_CliffCastInfo.Point.x - m_Position.x, m_CliffCastInfo.Point.z - m_Position.z)));
	camera->ChangeState(ECameraState::CAMERA_RESET);
	ResetJumpPower();
	DisableAttackSphere();
}

bool CPlayer::IsAnimationEndPlayer(const EPlayerAnimationNumber AnimationNumber) const
{
	return IsAnimationEnd<EPlayerAnimationNumber>(AnimationNumber);
}

bool CPlayer::IsCliffUpTrigger() const
{
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return false;
	}
	return instance->IsCliffUpTrigger();
}

bool CPlayer::IsCliffDownTrigger() const
{
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return false;
	}
	return instance->IsCliffDownTrigger();
}

bool CPlayer::CliffUpMove(float CountTime)
{
	if (CountTime == 0.0f)
	{
		m_CliffUpPosition.x = m_Position.x + m_Front.x;
		m_CliffUpPosition.y = m_Position.y + ADD_GOAL_CLIFFUP_HEIGHT;
		m_CliffUpPosition.z = m_Position.z + m_Front.z;
		PlaySoundIndex(ESoundIndex::CLIFF_UP);
		return false;
	}
	if (CountTime < CLIFFUP_MOVETIME)
	{
		m_Position.x = std::lerp(m_Position.x, m_CliffUpPosition.x, CLIFFUP_XZ_MOVETIME);
		m_Position.y = std::lerp(m_Position.y, m_CliffUpPosition.y, CLIFFUP_Y_MOVETIME);
		m_Position.z = std::lerp(m_Position.z, m_CliffUpPosition.z, CLIFFUP_XZ_MOVETIME);
		return false;
	}
	return true;
}

void CPlayer::ReverseRotationYaw()
{
	SetRotationYaw(m_Rotation.y - 180.0f);
}

bool CPlayer::IsSpinJumpTrigger() const
{
	if (m_IsPlaySpinJumpAction)
	{
		return false;
	}
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return false;
	}
	return instance->IsSpinJumpTrigger();
}

bool CPlayer::SpinJumpMove(const float CountTime, const float DeltaTime)
{
	if (CountTime < SPINJUMP_MOVETIME)
	{//スピンジャンプ時は少し上昇させる
		if (!IsUpWallHit())
		{
			m_Position.y += SPINJUMP_MOVESPEED * DeltaTime;
		}
		return false;
	}
	return true;
}

bool CPlayer::CheckWallSlip() const
{
	if (!m_FrontLine)
	{
		return false;
	}
	if (!m_StateMachine)
	{
		return false;
	}
	const EPlayerStateType playerState = m_StateMachine->GetCurrentState();
	const bool isWallSlipState = playerState == EPlayerStateType::AIR_WALL_SLIP;

	//壁滑り時の時以外はオブジェクトに当たっていなければ終了
	if (!isWallSlipState)
	{
		if (!m_IsCapsuleHit)
		{
			return false;
		}
	}

	Vector3 lineCheckFront = m_Front;
	//モーション側で正面方向を反転しているためラインのチェック方向も反転
	if (IsTurnJumpState() || IsCrouchingJumpState())
	{
		 lineCheckFront *= -1.0f;
	}

	//壁に対して移動入力されているか
	//ただし壁ジャンプ関連のアクションは移動入力がされていなくても壁滑りにする
	const bool isNoInputDotCheck = playerState == EPlayerStateType::AIR_WALL_JUMP || isWallSlipState;
	if (!isNoInputDotCheck)
	{
		const Vector2 inputDirection = GetInputDirection();
		const float inputWallDot = Vector3::Dot(Vector3(inputDirection.x, 0.0f, inputDirection.y), lineCheckFront);
		if (inputWallDot < WALLSLIP_INPUT_DOT)
		{
			return false;
		}
	}

	RayCastInfo frontCastInfo;
	const float lineCheckRate = 2.0f;
	lineCheckFront *= lineCheckRate;
	const bool isLineHit = m_FrontLine->LineHitCheck(ECheckCollisionType::PLAYER, Vector3(m_Position.x, m_Position.y + ADD_WALLSILIPLINE_HEIGHT, m_Position.z), m_Position + lineCheckFront + Vector3(0.0f, ADD_WALLSILIPLINE_HEIGHT, 0.0f), &frontCastInfo);
	if (!isLineHit)
	{
		return false;
	}
	//対象のオブジェクトが壁滑り無効ならreturn
	if (frontCastInfo.CollisionTag == ECollisionTagName::NO_WALL_HIT)
	{
		return false;
	}
	lineCheckFront.Normalize();
	const float dot = Vector3::Dot(frontCastInfo.Normal, lineCheckFront);
	//壁に対して向いているか返す 壁滑り状態の際は壁に対して向いているかだけ確認
	if (isWallSlipState)
	{
		return dot < 0.0f;
	}
	return dot < WALLSLIP_WALL_ANGLE_DOT;
}

Vector3 CPlayer::Slip(const Vector3& L, const Vector3& N)
{
	Vector3 s; //滑りベクトル（滑る方向）

	//滑りベクトル S=L-(N * L)/(|N|^2)*N
	s = L - ((Vector3::Dot(N, (Vector3)L)) / (pow(N.Length(), 2.0f))) * N;

	return s;
}

void CPlayer::StartWallSlip()
{
	ResetJumpPower();
	ResetSpeed();
	PlaySoundIndex(ESoundIndex::WALL_SLIP);
	DisableAttackSphere();
}

void CPlayer::DestroyFinalJumpSE()
{
	DeleteSoundIndex(ESoundIndex::FINAL_JUMP);
}

void CPlayer::CheckLandMoveAnimation()
{
	SpeedParameter& speedParam = m_SpeedParameterList[EPlayerSpeedParameterType::LAND_MOVE];
	if (m_CurrentSpeed > speedParam.MaxSpeed - LANDMOVE_RUN_SPEED_MARGIN)
	{
		PlayAnimationPlayer(EPlayerAnimationNumber::RUN, false);
	}
	else
	{
		PlayAnimationPlayer(EPlayerAnimationNumber::WALK, false);
	}
}

void CPlayer::BackJumpMove(const EPlayerSpeedParameterType Type, const float DeltaTime)
{
	FrontMove(Type, DeltaTime, true);
	FallMove(DeltaTime);
}

void CPlayer::CheckUpWallHit()
{
	if (IsJumpUp())
	{
		//ジャンプ上昇中に天井に当たったら即落下状態にする
		if (IsUpWallHit())
		{
			ResetJumpPower();
		}
	}
}

void CPlayer::DestroyWallSlipSE()
{
	DeleteSoundIndex(ESoundIndex::WALL_SLIP);
}

void CPlayer::WallSlipMove(const float DeltaTime)
{
	m_Position.y -= WALL_SLIP_SPEED * DeltaTime;
}

void CPlayer::StartWallJump()
{
	PlaySoundIndex(ESoundIndex::WALL_JUMP);
	ReverseRotationYaw();
	m_JumpPower = WALL_JUMP_POWER;
}

bool CPlayer::IsDiveJumpTrigger() const
{
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return false;
	}
	return instance->IsDiveJumpTrigger();
}

void CPlayer::StartDiveJump()
{
	PlayEffect(EEffectType::DASH, Vector3(m_Position.x, m_Position.y + ADD_DIVEJUMP_EFFECT_HEIGHT, m_Position.z), DIVEJUMP_EFFECT_SIZE);
	PlaySoundIndex(ESoundIndex::DIVE);
	m_JumpState = ENormalJumpState::NO_JUMP;
	//スティックを傾けている方に即時回転(入力がなければ現在の物を使用)
	const Vector2 inputDirection = GetInputDirection();
	if (inputDirection.LengthSq() != 0.0f)
	{
		SetRotationYaw(GetAtan2Rotation(inputDirection));
	}
	m_JumpPower = DIVE_JUMP_POWER;
	
	if (m_AttackSphere)
	{
		m_AttackSphere->SetRadius(DIVEJUMP_ATTACK_COLLISION_SIZE);
		m_AttackSphere->SetAddSpherePosition(Vector3(0, DIVEJUMP_ATTACK_COLLISION_HEIGHT, 0));
		m_AttackSphere->Update(m_Position);
	}
}

void CPlayer::DiveJumpMove(const float DeltaTime)
{
	m_Position.y += m_JumpPower * GetTimeScale();
	// 上昇中かつキーが押されている場合は下降量を減らす
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}

	// 落下量調整 ダイブジャンプの場合はボタン押下で飛距離が伸びる
	if (instance->IsDiveJumpPress() && m_JumpPower > 0.0f)
	{
		m_JumpPower -= GRAVITY_POWER * DeltaTime;
	}
	else if (m_JumpPower > MAX_DOWN_SPEED)
	{
		m_JumpPower -= POWER_GRAVITY_POWER * DeltaTime;
	}

	FrontMove(EPlayerSpeedParameterType::DIVE_JUMP, DeltaTime);

	if (m_AttackSphere)
	{
		m_AttackSphere->Update(m_Position + m_Front);
	}
}

bool CPlayer::IsWallHit()
{
	RayCastInfo frontCastInfo;
	if (!m_FrontLine->LineHitCheck(ECheckCollisionType::PLAYER, m_Position, m_Position + m_Front, &frontCastInfo))
	{
		return false;
	}

	const float dot = fabs(Vector3::Dot(frontCastInfo.Normal, m_Front));

	return dot >= WALL_HIT_DOT;
}

void CPlayer::StartWallHit()
{
	ResetJumpPower();
	//死亡時は落下のみ行うためHit関連の処理を行わない
	if (IsDeath())
	{
		return;
	}
	PlayHitStop(EHitStopType::LOW);
	PlayVibration(EVibrationType::MIDDLE);
	PlayCameraShake(ECameraShakeType::MIDDLE);
	PlayEffect(EEffectType::WALL_HIT, m_Position, WALLHIT_EFFECT_SIZE);
	PlaySoundIndex(ESoundIndex::WALL_HIT);
}

bool CPlayer::IsHipDropTrigger() const
{
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return false;
	}
	return instance->IsHipDropTrigger();
}

void CPlayer::StartHipDrop()
{
	PlaySoundIndex(ESoundIndex::HIPDROP_START);
	ResetJumpPower();
	ResetSpeed();
	DisableAttackSphere();
}

void CPlayer::HipDropMove(const float DeltaTime)
{
	m_Position.y += m_JumpPower * GetTimeScale();
	if (m_JumpPower > MAX_HIPDROP_DOWN_SPEED)
	{
		m_JumpPower -= POWER_GRAVITY_POWER * DeltaTime;
	}
	UpdateFallAttackSphere();
}

void CPlayer::SetJumpAnimationAndJumpPower()
{
	m_LandedState = ELandedState::AIR;
	//3段ジャンプは移動速度が一定以上の時のみ
	if (m_JumpState == ENormalJumpState::SECOND_JUMP)
	{
		if (REQUIRED_FINALJUMP_MOVE_SPEED >= m_CurrentSpeed)
		{
			m_JumpState = ENormalJumpState::NO_JUMP;
		}
	}

	switch (m_JumpState)
	{
		case ENormalJumpState::NO_JUMP:
		{
			PlayAnimationPlayer(EPlayerAnimationNumber::JUMP_START);
			m_JumpState = ENormalJumpState::FIRST_JUMP;
			m_JumpPower = NORMAL_JUMP_POWER;
			PlaySoundIndex(ESoundIndex::FIRST_JUMP);
		};
		break;
		case ENormalJumpState::FIRST_JUMP:
		{
			PlayAnimationPlayer(EPlayerAnimationNumber::SECOND_JUMP);
			m_JumpState = ENormalJumpState::SECOND_JUMP;
			m_JumpPower = SECOND_JUMP_POWER;
			PlaySoundIndex(ESoundIndex::SECOND_JUMP);
		};
		break;
		case ENormalJumpState::SECOND_JUMP:
		{
			PlayAnimationPlayer(EPlayerAnimationNumber::FINAL_JUMP_START);
			m_JumpState = ENormalJumpState::NO_JUMP;
			m_JumpPower = FINAL_JUMP_POWER;
			PlaySoundIndex(ESoundIndex::SECOND_JUMP);
		};
		break;
		case ENormalJumpState::HIGH_JUMP:
		{
			PlayAnimationPlayer(EPlayerAnimationNumber::HIGH_JUMP);
			PlaySoundIndex(ESoundIndex::HIPDROP_JUMP);
			m_JumpPower = HIGH_JUMP_POWER;
			m_JumpState = ENormalJumpState::NO_JUMP;
			//モーション側で回転を行っているため意図しない補間を行いようにする
			SetNotBlendAnimation(true);
		};
		break;
	}
}

bool CPlayer::IsTailAttackTrigger() const
{
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return false;
	}
	return instance->IsTailAttackTrigger();
}

void CPlayer::StartTailAttack()
{
	PlaySoundIndex(ESoundIndex::TAIL_ATTACK);
	DisableAttackSphere();
}

void CPlayer::UpdateTailAttackCollision(const float CountTime)
{
	if (!m_AttackSphere)
	{
		return;
	}

	if (CountTime >= TAIL_ATTACK_START_TIME && CountTime <= TAIL_ATTACK_END_TIME)
	{
		m_AttackSphere->SetRadius(TAILATTACK_ATTACK_COLLISION_SIZE);
		m_AttackSphere->SetAddSpherePosition(Vector3(m_Front.x, TAILATTACK_ATTACK_COLLISION_HEIGHT, m_Front.z));
	}
	else
	{
		DisableAttackSphere();
	}
	m_AttackSphere->Update(m_Position);
}

void CPlayer::DisableAttackSphere()
{
	if (m_AttackSphere)
	{
		m_AttackSphere->SetRadius(0.0f);
	}
}

void CPlayer::UpdateFallAttackSphere()
{
	if (!m_AttackSphere)
	{
		return;
	}
	if (IsFall())
	{
		m_AttackSphere->SetRadius(FALL_ATTACK_COLLISION_SIZE);
		m_AttackSphere->SetAddSpherePosition(Vector3(0, FALL_ATTACK_COLLISION_HEIGHT, 0));
		m_AttackSphere->Update(m_Position);
	}
}

bool CPlayer::IsTurnInput()
{
	if (!IsInputMove())
	{
		return false;
	}
	//今のプレイヤーの向きより後ろに入力がされていれば反転入力と判定
	const Vector2 inputDirection = GetInputDirection();
	const float inputRotation = GetAtan2Rotation(inputDirection);
	const float differenceRotation = fabs((m_Rotation.y) - (inputRotation));
	const bool isTurn = differenceRotation >= TURN_START_ROTATION && differenceRotation <= TURN_END_ROTATION;
	return isTurn;
}

void CPlayer::StartTurnJump()
{
	m_LandedState = ELandedState::AIR;
	m_JumpPower = TURN_JUMP_POWER;
	PlaySoundIndex(ESoundIndex::MOVE_CHANGE_JUMP);
}

bool CPlayer::IsTurnJumpState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::LAND_TURN_JUMP;
}

bool CPlayer::IsCrouchingJumpState() const
{
	if (!m_StateMachine)
	{
		return false;
	}
	return m_StateMachine->GetCurrentState() == EPlayerStateType::LAND_CROUCHING_JUMP;
}

bool CPlayer::IsCrouchingPress() const
{
	CInputInterface* instance = CInputInterface::GetInstance()->GetInstance();
	if (!instance)
	{
		return false;
	}
	return instance->IsCrouchingPress();
}

void CPlayer::StartCrouchingJump()
{
	m_LandedState = ELandedState::AIR;
	m_JumpPower = CROUCHING_JUMP_POWER;
	PlaySoundIndex(ESoundIndex::CROUCHING_JUMP);
}

void CPlayer::StartDashJump()
{
	PlaySoundIndex(ESoundIndex::DASH_JUMP);
	m_JumpPower = DASH_JUMP_POWER;
}

bool CPlayer::IsSlopeSlip() const
{
	if (m_DownCastInfo.CollisionTag == ECollisionTagName::NO_SLOPESLIP_HIT)
	{
		return false;
	}
	const float minusAngle = 90.0f;
	const float meshAngle = fabs(m_DownCastInfo.Normal.Angle() - minusAngle);
	return meshAngle > SLIP_FLOOR;
}

bool CPlayer::CanSlopeSlideJump() const
{
	if (m_CurrentAnimationNumber != static_cast<int>(EPlayerAnimationNumber::GROUND_SLIP))
	{
		return false;
	}
	return IsJumpTrigger();
}

void CPlayer::PlaySpinJumpAction()
{
	m_IsPlaySpinJumpAction = true;
	PlaySoundIndex(ESoundIndex::SPIN_JUMP);
}

bool CPlayer::StartSlopeSlip()
{
	bool isCheckGroundSlipAnimation = false;
	if (m_CurrentAnimationNumber == static_cast<int>(EPlayerAnimationNumber::HIPDROP) || m_CurrentAnimationNumber == static_cast<int>(EPlayerAnimationNumber::HIPDROP_END))
	{
		PlayAnimationPlayer(EPlayerAnimationNumber::GROUND_SLIP);
	}
	else if (m_CurrentAnimationNumber == static_cast<int>(EPlayerAnimationNumber::DIVE_JUMP))
	{
		PlayAnimationPlayer(EPlayerAnimationNumber::SLIP_DIVE);
	}
	else
	{
		const float dot = Vector3::Dot(m_WallNormal, m_Front);
		//マイナスで坂に対して前を向いている
		if (dot > 0.0f)
		{
			PlayAnimationPlayer(EPlayerAnimationNumber::GROUND_SLIP);
		}
		else
		{
			PlayAnimationPlayer(EPlayerAnimationNumber::SLIP_DIVE);
		}
		isCheckGroundSlipAnimation = true;
	}
	PlayGroundSlipSE();
	ResetSpeed();
	m_SlipGravity = Vector3::Zero();
	return isCheckGroundSlipAnimation;
}

bool CPlayer::SlopeSlipMove(const bool IsCheckGroundSlipAnim, const float DeltaTime)
{
	bool isAnimationChange = false;
	if (IsCheckGroundSlipAnim && m_CurrentSpeed < fabs(m_SlipGravity.y))
	{
		const float dot = Vector3::Dot(m_DownCastInfo.Normal, m_Front);
		//マイナスで坂に対して前を向いている
		if (dot > 0.0f)
		{
			PlayAnimationPlayer(EPlayerAnimationNumber::GROUND_SLIP);
		}
		else
		{
			if (m_CurrentAnimationNumber != static_cast<int>(EPlayerAnimationNumber::SLIP_DIVE))
			{
				PlayAnimationPlayer(EPlayerAnimationNumber::SLIP_DIVE);
			}
		}
		isAnimationChange = true;
	}
	const Vector3 slipVector = Slip(m_SlipGravity, m_DownCastInfo.Normal);
	m_SaveDownNormal = m_DownCastInfo.Normal;
	//重力を加算
	m_SlipGravity.y -= SLIP_SPEED * DeltaTime;
	m_Position += slipVector;
	return isAnimationChange;
}

bool CPlayer::GroundSlipMove(const float DeltaTime)
{
	if (m_SlipGravity.y >= 0.0f)
	{
		return true;
	}
	const Vector3 slipVector = Slip(m_SlipGravity, m_SaveDownNormal);
	m_SlipGravity.y += SLIP_SPEED * DeltaTime;
	m_Position += slipVector;
	return false;
}

void CPlayer::JumpSlipMove(const float DeltaTime)
{
	if (!IsSlopeSlip())
	{
		return;
	}
	if (m_SlipGravity.y >= 0.0f)
	{
		return;
	}
	const Vector3 slipVector = Slip(m_SlipGravity, m_SaveDownNormal);
	m_Position.x += slipVector.x;
	m_Position.z += slipVector.z;
}

void CPlayer::PlayDeathAnimeSE()
{
	PlaySoundIndex(ESoundIndex::DEATH_MOVE);
}

void CPlayer::CheckMoveSE(float& CountSoundTime)
{
	float interval = 0.0f;
	if (m_CurrentAnimationNumber == static_cast<int>(EPlayerAnimationNumber::RUN))
	{
		interval = RUN_SE_INTERVAL;
	}
	else
	{
		interval = WALK_SE_INTERVAL;
	}
	if (CountSoundTime > interval)
	{
		PlayMoveSE();
		CountSoundTime = 0;
	}
}

void CPlayer::PlayMoveSE()
{
	switch (m_DownCastInfo.StepsCollisionTag)
	{
	case ECollisionTagSteps::NORMAL:
		PlaySoundIndex(ESoundIndex::FOOTSTEPS_NORMAL);
		break;
	case ECollisionTagSteps::BRIDGE:
		PlaySoundIndex(ESoundIndex::FOOTSTEPS_BRIDGE);
		break;
	case ECollisionTagSteps::ROCK:
		PlaySoundIndex(ESoundIndex::FOOTSTEPS_ROCK);
		break;
	}
}

void CPlayer::PlayGroundSlipSE()
{
	PlaySoundIndex(ESoundIndex::GROUND_SLIP);
}

void CPlayer::TurnTowardsCamera()
{
	CCamera* camera = GetCamera();
	if (!camera)
	{
		return;
	}
	const Vector3 cameraPosition = camera->GetPosition();
	const float x = cameraPosition.x - m_Position.x;
	const float z = cameraPosition.z - m_Position.z;
	m_Rotation.y = GetAtan2Rotation(Vector2(x, z));
	UpdateVector();
}

void CPlayer::DestroyGroundSlipSE()
{
	DeleteSoundIndex(ESoundIndex::GROUND_SLIP);
}

void CPlayer::DrawGetStar()
{
	if (!m_StarModel)
	{
		return;
	}
	if (!m_EnableGetStarDraw)
	{
		return;
	}
	XMMATRIX world;
	world = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	world *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
	world *= XMMatrixTranslation(m_Position.x + m_Front.x, m_Position.y + ADD_GETSTAR_HEIGHT, m_Position.z + m_Front.z);
	m_StarModel->ShadowOnModelDraw(EShadowType::DOWN_SHADOW, world);
}

void CPlayer::StartCongratulation()
{
	ResetSpeed();
	PlayVibration(EVibrationType::MIDDLE);
	TurnTowardsCamera();
	PlaySoundIndex(ESoundIndex::GETSTAR);
	StopBGM();
}

bool CPlayer::EndCongratulation()
{
	bool isEndCongratulation = false;
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return false;
	}
	CStarDoor* starDoor = scene.lock()->GetGameObject<CStarDoor>(ESceneObjectType::MESH_OBJECT);
	if (!starDoor ||
		!starDoor->IsStartStarDoorAnimation(m_MyStarCount) ||
		starDoor->IsEndStarDoorAnimation())
	{
		ReplayBGM();
		isEndCongratulation = true;
	}
	return isEndCongratulation;
}

void CPlayer::ChangeCongratulationState()
{
	if (!m_StateMachine)
	{
		return;
	}
	m_StateMachine->ChangeState(EPlayerStateType::COMMON_CONGRATULATION);
}

void CPlayer::ChangeFallState()
{
	if (!m_StateMachine)
	{
		return;
	}
	m_StateMachine->ChangeState(EPlayerStateType::AIR_FALL);
}

void CPlayer::ChangeNormalJumpState(const ENormalJumpState JumpState)
{
	if (!m_StateMachine)
	{
		return;
	}
	SetJumpState(JumpState);
	m_StateMachine->ChangeState(EPlayerStateType::LAND_NORMAL_JUMP, true);
}

CCapsule* CPlayer::GetCapsule() const
{
	return m_Capsule.get();
}

CSphere* CPlayer::GetAttackSphere() const
{
	return m_AttackSphere.get();
}
