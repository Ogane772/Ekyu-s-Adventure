#pragma once
#include "../../../../FrameWork/State/StateBase.h"
class CPlayer;
enum class EPlayerStateType : int;

class CPlayerStateBase : public CStateBase<CPlayer, EPlayerStateType>
{
public:
	CPlayerStateBase() {};
	CPlayerStateBase(CPlayer* InOwner);

	virtual EPlayerStateType Update(const float DeltaTime) override;
};
