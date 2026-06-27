#pragma once

template<class TOwner, class TStateType>
class CStateBase
{
public:
    CStateBase() {};
    CStateBase(TOwner* InOwner) : m_Owner(InOwner) {};
    virtual ~CStateBase() = default;

    virtual void Enter() {};
    virtual TStateType Update(const float DeltaTime) { return TStateType{}; }
    virtual void Exit() {};

protected:
    TOwner* m_Owner = nullptr;
};