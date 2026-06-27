#include "PlayerStateLandTurn.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateLandTurn::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::MOVE_CHANGE);
        m_Owner->PlayGroundSlipSE();
    }
}

EPlayerStateType CPlayerStateLandTurn::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }
    const ELandedState landState = m_Owner->CheckGround();
    if (landState == ELandedState::AIR)
    {
        return EPlayerStateType::AIR_FALL;
    }

    m_Owner->FrontMove(EPlayerSpeedParameterType::LAND_TURN, DeltaTime);

    if(m_Owner->IsZeroSpeed())
    {
        if (m_Owner->IsInputMove())
        {
            m_Owner->ReverseRotationYaw();
        }
        return EPlayerStateType::LAND_IDLE;
    }

    if (m_Owner->IsJumpTrigger())
    {
        return EPlayerStateType::LAND_TURN_JUMP;
    }

    return EPlayerStateType::LAND_TURN;
}

void CPlayerStateLandTurn::Exit()
{
    if (m_Owner)
    {
        m_Owner->DestroyGroundSlipSE();
    }
}
