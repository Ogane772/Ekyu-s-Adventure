#include "PlayerStateAirWallSlip.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateAirWallSlip::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::WALL_SLIP);
        m_Owner->StartWallSlip();
    }
}

EPlayerStateType CPlayerStateAirWallSlip::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    m_Owner->WallSlipMove(DeltaTime);

    if (m_Owner->IsJumpTrigger())
    {
        return EPlayerStateType::AIR_WALL_JUMP;
    }

    if (m_Owner->CheckGround() != ELandedState::AIR)
    {
        m_Owner->OnLanding();
        return EPlayerStateType::LAND_IDLE;
    }

    if (!m_Owner->CheckWallSlip())
    {
        return EPlayerStateType::LAND_IDLE;
    }

    return EPlayerStateType::AIR_WALL_SLIP;
}

void CPlayerStateAirWallSlip::Exit()
{
    if (m_Owner)
    {
        m_Owner->DestroyWallSlipSE();
    }
}
