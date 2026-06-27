#include "PlayerStateLandSlopeSlip.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateLandSlopeSlip::Enter()
{
    if (m_Owner)
    {
        IsCheckGroundSlipAnimation = m_Owner->StartSlopeSlip();
    }
}

EPlayerStateType CPlayerStateLandSlopeSlip::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    m_Owner->UpdateLanding(DeltaTime);

    //坂滑り中か坂滑り後の地面滑り中かで移動を変える
    if (m_Owner->IsSlopeSlip())
    {
        m_Owner->SlopeSlipMove(IsCheckGroundSlipAnimation, DeltaTime);
    }
    else
    {
        if (m_Owner->GroundSlipMove(DeltaTime))
        {
            return EPlayerStateType::LAND_IDLE;
        }
    }

    if (m_Owner->CheckGround() == ELandedState::AIR)
    {
        return EPlayerStateType::AIR_FALL;
    }

    //座り込む坂滑りならジャンプ可能。地面滑り中なら何時でもジャンプ可能
    if (m_Owner->IsSlopeSlip())
    {
        if (m_Owner->CanSlopeSlideJump())
        {
            return EPlayerStateType::LAND_NORMAL_JUMP;
        }
    }
    else
    {
        if (m_Owner->IsJumpTrigger())
        {
            return EPlayerStateType::LAND_NORMAL_JUMP;
        }
    }
    return EPlayerStateType::LAND_SLOPE_SLIP;
}

void CPlayerStateLandSlopeSlip::Exit()
{
    if (m_Owner)
    {
        m_Owner->DestroyGroundSlipSE();
    }
}
