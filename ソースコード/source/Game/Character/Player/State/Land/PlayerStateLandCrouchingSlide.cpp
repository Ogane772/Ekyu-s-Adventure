#include "PlayerStateLandCrouchingSlide.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

namespace
{
    constexpr float SLIDETIME = 0.6f;
};

void CPlayerStateLandCrouchingSlide::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::CROUCHING);
        m_Owner->PlayGroundSlipSE();
    }
}

EPlayerStateType CPlayerStateLandCrouchingSlide::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    m_Owner->InputMove(EPlayerSpeedParameterType::CROUCHING_SLIDE, DeltaTime);

    m_Owner->UpdateLanding(DeltaTime);

    if (m_Owner->CheckGround() == ELandedState::AIR)
    {
        return EPlayerStateType::AIR_FALL;
    }

    if (!m_Owner->IsCrouchingPress())
    {
        return EPlayerStateType::LAND_IDLE;
    }

    if (m_Owner->IsZeroSpeed())
    {
        if (m_Owner->IsInputMove())
        {
            return EPlayerStateType::LAND_CROUCHING_MOVE;
        }
        return EPlayerStateType::LAND_CROUCHING_IDLE;
    }

    if (m_Owner->IsJumpTrigger())
    {
        return EPlayerStateType::LAND_DASH_JUMP;
    }
    if (m_Owner->IsTailAttackTrigger())
    {
        return EPlayerStateType::LAND_TAIL_ATTACK;
    }

    return EPlayerStateType::LAND_CROUCHING_SLIDE;
}

void CPlayerStateLandCrouchingSlide::Exit()
{
    if (m_Owner)
    {
        m_Owner->DestroyGroundSlipSE();
    }
}
