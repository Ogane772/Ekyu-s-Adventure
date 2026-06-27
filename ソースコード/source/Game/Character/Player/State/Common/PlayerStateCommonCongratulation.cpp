#include "PlayerStateCommonCongratulation.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

namespace
{
    constexpr float START_DRAW_STAR_TIME = 0.7f;
    constexpr float END_CONGRATULATION_TIME = 2.1f;
};

void CPlayerStateCommonCongratulation::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::GET_STAR, false);
        m_Owner->StartCongratulation();
    }
    m_IsStartEndCongratulation = false;
    m_CountCongratulationTime = 0.0f;
}

EPlayerStateType CPlayerStateCommonCongratulation::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    if (m_CountCongratulationTime > START_DRAW_STAR_TIME)
    {
        m_Owner->SetEnableGetStarDraw(true);
    }

    if (m_CountCongratulationTime > END_CONGRATULATION_TIME)
    {
        if (!m_IsStartEndCongratulation)
        {
            m_Owner->StartStarDoorAnimation();
            m_IsStartEndCongratulation = true;
        }
        if (m_Owner->EndCongratulation())
        {
            return EPlayerStateType::AIR_FALL;
        }
    }

    m_CountCongratulationTime += DeltaTime;

    return EPlayerStateType::COMMON_CONGRATULATION;
}

void CPlayerStateCommonCongratulation::Exit()
{
    if (m_Owner)
    {
        m_Owner->SetEnableGetStarDraw(false);
    }
}
