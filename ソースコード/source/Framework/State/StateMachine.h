#pragma once
//ステートマシンの親クラス
#include <memory>
#include <unordered_map>
#include "StateBase.h"

template<class TOwner, class TStateType>
class CStateMachine
{
public:
    CStateMachine(TOwner* InOwner)
        : m_Owner(InOwner)
    {
    }

    ~CStateMachine()
    {
        if (m_CurrentState.expired())
        {
            return;
        }

        m_CurrentState.lock()->Exit();
    }

    void Update(const float DeltaTime)
    {
        if (m_CurrentState.expired())
        {
            return;
        }
        
        //ステートの更新確認
        const TStateType nextStateType = m_CurrentState.lock()->Update(DeltaTime);
        m_CountStateTime += DeltaTime;

        //ステートが変更されていたら終了処理を行いステートを変更する
        if (nextStateType == m_CurrentStateType)
        {
            return;
        }

        ChangeState(nextStateType);
    }

    void ChangeState(const TStateType NextStateType, const bool IsIgnoreSameState = false)
    {
        if (!IsIgnoreSameState)
        {
            if (m_CurrentStateType == NextStateType)
            {
                return;
            }
        }

        if (!m_CurrentState.expired())
        {
            m_CurrentState.lock()->Exit();
        }

        if (!m_StateList.contains(NextStateType))
        {
            return;
        }
        m_PrevStateType = m_CurrentStateType;
        std::weak_ptr<CStateBase<TOwner, TStateType>> nextState = m_StateList[NextStateType];
        if (nextState.expired())
        {
            return;
        }
        m_CountStateTime = 0.0f;
        nextState.lock()->Enter();
        m_CurrentState = nextState;
        m_CurrentStateType = NextStateType;
    }

    TStateType GetCurrentState() const { return m_CurrentStateType; }
    TStateType GetPrevState() const { return m_PrevStateType; }
    float GetCountStateTime() const { return m_CountStateTime; }

protected:
    TOwner* m_Owner = nullptr;

    std::weak_ptr<CStateBase<TOwner, TStateType>> m_CurrentState;

    std::unordered_map<TStateType, std::shared_ptr<CStateBase<TOwner, TStateType>>> m_StateList;

    TStateType m_CurrentStateType{};
    TStateType m_PrevStateType{};
    float m_CountStateTime = 0.0f;
};