#include "PlayerStateAirHipDrop.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

namespace
{
    constexpr float HIPDROP_END_TIME = 0.4f;
};

CPlayerStateAirHipDrop::CPlayerStateAirHipDrop(CPlayer* InOwner)
    : CPlayerStateBase(InOwner)
{
    m_ReturnState = EPlayerStateType::AIR_HIPDROP;
    m_StateMachine.AddState<CPlayerStateAirHipDrop>(
        EHipDropState::NONE,
        this,
        nullptr,
        nullptr,
        nullptr
    );
    m_StateMachine.AddState<CPlayerStateAirHipDrop>(
        EHipDropState::START,
        this,
        &CPlayerStateAirHipDrop::HipDropStart,
        &CPlayerStateAirHipDrop::HipDropStartUpdate,
        nullptr
    );
    m_StateMachine.AddState<CPlayerStateAirHipDrop>(
        EHipDropState::END,
        this,
        &CPlayerStateAirHipDrop::HipDropEndStart,
        &CPlayerStateAirHipDrop::HipDropEndUpdate,
        nullptr
    );
    m_StateMachine.SetInitialState(EHipDropState::NONE);
}

void CPlayerStateAirHipDrop::Enter()
{
    m_ReturnState = EPlayerStateType::AIR_HIPDROP;
    m_StateMachine.ChangeState(EHipDropState::START);
}

EPlayerStateType CPlayerStateAirHipDrop::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    m_StateMachine.Update(DeltaTime);

    return m_ReturnState;
}

void CPlayerStateAirHipDrop::HipDropStart()
{
    if (!m_Owner)
    {
        return;
    }
    m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::HIPDROP);
    m_Owner->StartHipDrop();
}

void CPlayerStateAirHipDrop::HipDropStartUpdate(float DeltaTime)
{
    if (!m_Owner)
    {
        return;
    }
    if (!m_Owner->IsAnimationEndPlayer(EPlayerAnimationNumber::HIPDROP))
    {
        return;
    }
    m_Owner->HipDropMove(DeltaTime);
    if (m_Owner->CheckGround() != ELandedState::AIR)
    {
        m_Owner->OnLanding();
        if (m_Owner->IsSlopeSlip())
        {
            m_Owner->SetNotBlendAnimation(true);
            m_StateMachine.ChangeState(EHipDropState::NONE);
            m_ReturnState = EPlayerStateType::LAND_SLOPE_SLIP;
            return;
        }
        m_StateMachine.ChangeState(EHipDropState::END);
    }
}

void CPlayerStateAirHipDrop::HipDropEndStart()
{
    if (!m_Owner)
    {
        return;
    }
    m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::HIPDROP_END, false);
}

void CPlayerStateAirHipDrop::HipDropEndUpdate(float DeltaTime)
{
    if (!m_Owner)
    {
        return;
    }
    m_Owner->UpdateLanding(DeltaTime);
    if (m_StateMachine.GetCurrentStateTime() > HIPDROP_END_TIME)
    {
        m_StateMachine.ChangeState(EHipDropState::NONE);
        m_ReturnState = EPlayerStateType::LAND_IDLE;
    }
    else
    {
        if (m_Owner->IsJumpTrigger())
        {
            m_Owner->SetJumpState(ENormalJumpState::HIGH_JUMP);
            m_ReturnState = EPlayerStateType::LAND_NORMAL_JUMP;
        }
    }
}
