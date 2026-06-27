#pragma once
#include "../PlayerStateBase.h"
enum class EPlayerStateType : int;

class CPlayerStateLandDashJump : public CPlayerStateBase
{
public:
	CPlayerStateLandDashJump(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};
	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;
};