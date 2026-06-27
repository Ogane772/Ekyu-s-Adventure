#pragma once
#include "../../../../FrameWork/State/StateMachine.h"
class CPlayer;
enum class EPlayerStateType;

class CPlayerStateMachine : public CStateMachine<CPlayer, EPlayerStateType>
{
public:
    CPlayerStateMachine(CPlayer* InOwner);
};