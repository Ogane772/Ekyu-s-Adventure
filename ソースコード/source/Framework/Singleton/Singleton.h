# pragma once

# include <cassert>
# include <mutex>
#include<memory>

class SingletonFinalizer 
{
public:
    using FinalizerFunc = void(*)();
    static void AddFinalizer(FinalizerFunc func);
    static void Finalize();
};

template <typename T>
class CSingleton
{
public:
    static T* GetInstance()
    {
        std::call_once(m_Init, Create);
        if (!m_Instance)
        {
            return nullptr;
        }
        return m_Instance.get();
    }

private:
    static void Create() 
    {
        m_Instance = std::make_unique<T>();
        SingletonFinalizer::AddFinalizer(&CSingleton<T>::Destroy);
    }

    static void Destroy() 
    {
        m_Instance.reset();
    }

    static std::once_flag m_Init;
    static std::unique_ptr<T> m_Instance;
};

template <typename T> std::once_flag CSingleton<T>::m_Init;
template <typename T> std::unique_ptr<T> CSingleton<T>::m_Instance = nullptr;