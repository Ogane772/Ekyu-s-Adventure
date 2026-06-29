#pragma once
//関数単位でステートを登録できるクラス
#include <iostream>
#include <unordered_map>
#include <functional>

template<typename State>
class CFunctionStateMachine
{
public:
    using EnterFunc = std::function<void()>;
    using UpdateFunc = std::function<void(float)>;
    using ExitFunc = std::function<void()>;
    using DrawFunc = std::function<void()>;

    struct StateFuncs
    {
        EnterFunc onEnter;
        UpdateFunc onUpdate;
        ExitFunc onExit;
        DrawFunc onDraw;
    };

    template<typename T>
    void AddState(const State State, T* Instance, void (T::* Enter)() = nullptr, void (T::* Update)(float) = nullptr, void (T::* Exit)() = nullptr, void (T::* Draw)() = nullptr)
    {
        if (!Instance)
        {
            return;
        }

        StateFuncs funcs;

        if (Enter) 
        {
            funcs.onEnter = [=]() { (Instance->*Enter)(); };
        }
        if (Update) 
        {
            funcs.onUpdate = [=](float DeltaTime) { (Instance->*Update)(DeltaTime); };
        }
        if (Exit) 
        {
            funcs.onExit = [=]() { (Instance->*Exit)(); };
        }
        if (Draw)
        {
            funcs.onDraw = [=]() { (Instance->*Draw)(); };
        }

        m_States[State] = funcs;
    }

    void SetInitialState(const State State)
    {
        m_CurrentState = State;
        if (m_States[State].onEnter) 
        {
            m_States[State].onEnter();
        }
    }

    void ChangeState(const State State, const bool IsIgnoreSameState = false)
    {
        if (!IsIgnoreSameState)
        {
            if (State == m_CurrentState)
            {
                return;
            }
        }

        if (m_States[m_CurrentState].onExit) 
        {
            m_States[m_CurrentState].onExit();
        }

        m_CurrentState = State;
        m_CurrentStateTime = 0.0f;

        if (m_States[m_CurrentState].onEnter)
        {
            m_States[m_CurrentState].onEnter();
        }
    }

    void Update(float DeltaTime) 
    {
        if (m_States[m_CurrentState].onUpdate) 
        {
            State currentState = m_CurrentState;
            m_States[m_CurrentState].onUpdate(DeltaTime);
            //onUpdate内でChangeStateが呼ばれた場合、m_CurrentStateが切り替わる都合上次のステートのm_CurrentStateTimeが進んでしまうため
            //onUpdateを呼んだステートと一致する場合のみ時間を進める
            if (currentState == m_CurrentState)
            {
                m_CurrentStateTime += DeltaTime;
            }
        }
    }

    void Draw()
    {
        if (m_States[m_CurrentState].onDraw)
        {
            m_States[m_CurrentState].onDraw();
        }
    }

    State GetState() const 
    {
        return m_CurrentState;
    }

    float GetCurrentStateTime() const { return m_CurrentStateTime; }

private:
    State m_CurrentState;
    std::unordered_map<State, StateFuncs> m_States;
    float m_CurrentStateTime = 0.0f;
};