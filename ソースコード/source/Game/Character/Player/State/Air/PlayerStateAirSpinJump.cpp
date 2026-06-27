#include "PlayerStateAirSpinJump.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateAirSpinJump::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::SPIN_JUMP);
        m_Owner->ResetJumpPower();
        m_Owner->DisableAttackSphere();
        m_Owner->PlaySpinJumpAction();
    }
    m_CountSpinJumpTime = 0.0f;
}

EPlayerStateType CPlayerStateAirSpinJump::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    if (m_Owner->IsInputMove())
    {
        m_Owner->InputMove(EPlayerSpeedParameterType::SPIN_JUMP, DeltaTime);
    }

    if (m_Owner->IsDiveJumpTrigger())
    {
        return EPlayerStateType::AIR_DIVE_JUMP;
    }

    const bool isSpinJumpMoveEnd = m_Owner->SpinJumpMove(m_CountSpinJumpTime, DeltaTime);
    if (isSpinJumpMoveEnd)
    {
        m_Owner->ResetSpeed();
        return EPlayerStateType::AIR_FALL;
    }
    else
    {
        m_CountSpinJumpTime += DeltaTime;
    }

    return EPlayerStateType::AIR_SPIN_JUMP;
}

void CPlayerStateAirSpinJump::Exit()
{
    if (m_Owner)
    {
        m_Owner->SetNotBlendAnimation(true);
    }
}
