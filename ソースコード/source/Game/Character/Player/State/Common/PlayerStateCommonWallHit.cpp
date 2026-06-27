#include "PlayerStateCommonWallHit.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

CPlayerStateCommonWallHit::CPlayerStateCommonWallHit(CPlayer* InOwner)
    : CPlayerStateBase(InOwner)
{
	m_ReturnState = EPlayerStateType::COMMON_WALL_HIT;
	m_StateMachine.AddState<CPlayerStateCommonWallHit>(
		EWallHitState::NONE,
		this,
		nullptr,
		nullptr,
		nullptr
	);
	m_StateMachine.AddState<CPlayerStateCommonWallHit>(
		EWallHitState::START,
		this,
		&CPlayerStateCommonWallHit::WallHitStart,
		&CPlayerStateCommonWallHit::WallHitStartUpdate,
		nullptr
	);
	m_StateMachine.AddState<CPlayerStateCommonWallHit>(
		EWallHitState::SIT,
		this,
		&CPlayerStateCommonWallHit::WallHitSitStart,
		&CPlayerStateCommonWallHit::WallHitSitUpdate,
		nullptr
	);
	m_StateMachine.AddState<CPlayerStateCommonWallHit>(
		EWallHitState::END,
		this,
		&CPlayerStateCommonWallHit::WallHitEndStart,
		&CPlayerStateCommonWallHit::WallHitEndUpdate,
		nullptr
	);
	m_StateMachine.SetInitialState(EWallHitState::NONE);
}

void CPlayerStateCommonWallHit::Enter()
{
	m_ReturnState = EPlayerStateType::COMMON_WALL_HIT;
	m_StateMachine.ChangeState(EWallHitState::START);
}

EPlayerStateType CPlayerStateCommonWallHit::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

	m_StateMachine.Update(DeltaTime);

    return m_ReturnState;
}

void CPlayerStateCommonWallHit::WallHitStart()
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::WALL_HIT_START);
	m_Owner->StartWallHit();
}

void CPlayerStateCommonWallHit::WallHitStartUpdate(float DeltaTime)
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->BackJumpMove(EPlayerSpeedParameterType::WALL_HIT_MOVE, DeltaTime);
	if (m_Owner->CheckGround() != ELandedState::AIR)
	{
		m_Owner->OnLanding();
		m_StateMachine.ChangeState(EWallHitState::SIT);
	}
}

void CPlayerStateCommonWallHit::WallHitSitStart()
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::WALL_HIT_SIT, false);
}

void CPlayerStateCommonWallHit::WallHitSitUpdate(float DeltaTime)
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->UpdateLanding(DeltaTime);
	if (m_Owner->IsAnimationEndPlayer(EPlayerAnimationNumber::WALL_HIT_SIT))
	{
		m_StateMachine.ChangeState(EWallHitState::END);
	}
}

void CPlayerStateCommonWallHit::WallHitEndStart()
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::WALL_HIT_END, false);
}

void CPlayerStateCommonWallHit::WallHitEndUpdate(float DeltaTime)
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->UpdateLanding(DeltaTime);
	if (m_Owner->IsAnimationEndPlayer(EPlayerAnimationNumber::WALL_HIT_END))
	{
		m_StateMachine.ChangeState(EWallHitState::NONE);
		if (m_Owner->IsDeath())
		{
			m_ReturnState = EPlayerStateType::COMMON_DEATH;
			return;
		}
		m_ReturnState = EPlayerStateType::LAND_IDLE;
	}
}

