#include "PlayerStateBase.h"
#include "PlayrStateTypeAndAnimationNumber.h"
#include "../Player.h"

CPlayerStateBase::CPlayerStateBase(CPlayer* InOwner)
	: CStateBase(InOwner)
{
}

EPlayerStateType CPlayerStateBase::Update(const float DeltaTime)
{
	return EPlayerStateType::MAX;
}
