#pragma once
#include "../PlayerStateBase.h"
enum class EPlayerStateType : int;

class CPlayerStateLandCrouchingIdle : public CPlayerStateBase
{
public:
	CPlayerStateLandCrouchingIdle(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};
	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;
};