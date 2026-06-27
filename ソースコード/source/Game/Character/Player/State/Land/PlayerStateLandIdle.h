#pragma once
#include "../PlayerStateBase.h"
enum class EPlayerStateType : int;

class CPlayerStateLandIdle : public CPlayerStateBase
{
public:
	CPlayerStateLandIdle(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};

	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;
};
