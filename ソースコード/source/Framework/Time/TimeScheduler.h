#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <algorithm>
//特定の時間到達時に登録した関数を実行するクラス
class CTimeScheduler 
{
public:

    void Add(const float PlayTime, std::function<void()> Func) { m_Tasks.emplace_back(PlayTime, Func); }

    void Run(float DeltaTime);

    void Reset() { m_CountTime = 0.0f; m_CurrentIndex = 0; }

    bool IsEndTask() const { return m_CurrentIndex >= m_Tasks.size(); }

    bool IsEmpty() const { return m_Tasks.empty(); }

private:
    std::vector<std::pair<float, std::function<void()>>> m_Tasks;
    float m_CountTime = 0.0f;
    int m_CurrentIndex = 0;
};