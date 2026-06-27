#include "PlayerStateAirCliff.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

CPlayerStateAirCliff::CPlayerStateAirCliff(CPlayer* InOwner)
    : CPlayerStateBase(InOwner)
{
	m_ReturnState = EPlayerStateType::AIR_CLIFF;
	m_StateMachine.AddState<CPlayerStateAirCliff>(
		ECliffState::NONE,
		this,
		nullptr,
		nullptr,
		nullptr
	);
	m_StateMachine.AddState<CPlayerStateAirCliff>(
		ECliffState::START,
		this,
		&CPlayerStateAirCliff::CliffStart,
		&CPlayerStateAirCliff::CliffStartUpdate,
		nullptr
	);
	m_StateMachine.AddState<CPlayerStateAirCliff>(
		ECliffState::LOOP,
		this,
		&CPlayerStateAirCliff::CliffLoopStart,
		&CPlayerStateAirCliff::CliffLoopUpdate,
		nullptr
	);
	m_StateMachine.AddState<CPlayerStateAirCliff>(
		ECliffState::UP,
		this,
		&CPlayerStateAirCliff::CliffUpStart,
		&CPlayerStateAirCliff::CliffUpUpdate,
		&CPlayerStateAirCliff::CliffUpEnd
	);
	m_StateMachine.SetInitialState(ECliffState::NONE);
}

void CPlayerStateAirCliff::Enter()
{
	m_ReturnState = EPlayerStateType::AIR_CLIFF;
	m_StateMachine.ChangeState(ECliffState::START);
	if (m_Owner)
	{
		m_Owner->ResetSpinJumpAction();
		m_Owner->ResetSpeed();
	}
}

EPlayerStateType CPlayerStateAirCliff::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

	m_StateMachine.Update(DeltaTime);

    return m_ReturnState;
}

void CPlayerStateAirCliff::Exit()
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->SetEnableCollision(true);
}

void CPlayerStateAirCliff::CliffStart()
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::CLIFF_START);
	m_Owner->StartCliff();
}

void CPlayerStateAirCliff::CliffStartUpdate(float DeltaTime)
{
	if (!m_Owner)
	{
		return;
	}
	if (m_Owner->IsAnimationEndPlayer(EPlayerAnimationNumber::CLIFF_START))
	{
		m_StateMachine.ChangeState(ECliffState::LOOP);
	}
}

void CPlayerStateAirCliff::CliffLoopStart()
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::CLIFF_LOOP);
}

void CPlayerStateAirCliff::CliffLoopUpdate(float DeltaTime)
{
	if (!m_Owner)
	{
		return;
	}

	if (m_Owner->IsCliffUpTrigger())
	{
		m_StateMachine.ChangeState(ECliffState::UP);
	}
	else if (m_Owner->IsCliffDownTrigger())
	{
		//‰º‚É~‚è‚éÛ‚É‚»‚Ì‚Ü‚Ü‚¾‚ÆÄ“x“¯‚¶ŠR‚É•ß‚Ü‚Á‚Ä‚µ‚Ü‚¤‚½‚ß”½“]‚³‚¹‚é
		m_Owner->ReverseRotationYaw();
		m_ReturnState = EPlayerStateType::LAND_MOVE;
	}
	else if (m_Owner->IsJumpTrigger())
	{
		m_StateMachine.ChangeState(ECliffState::NONE);
		m_ReturnState = EPlayerStateType::LAND_NORMAL_JUMP;
	}
}

void CPlayerStateAirCliff::CliffUpStart()
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::CLIFF_END);
	m_Owner->SetEnableCollision(false);
}

void CPlayerStateAirCliff::CliffUpUpdate(float DeltaTime)
{
	if (!m_Owner)
	{
		return;
	}
	const bool isMoveEnd = m_Owner->CliffUpMove(m_StateMachine.GetCurrentStateTime());
	if (isMoveEnd)
	{
		m_StateMachine.ChangeState(ECliffState::NONE);
		m_ReturnState = EPlayerStateType::LAND_IDLE;
	}
}

void CPlayerStateAirCliff::CliffUpEnd()
{
	if (!m_Owner)
	{
		return;
	}
	m_Owner->SetEnableCollision(true);
	m_Owner->SetNotBlendAnimation(true);
}
