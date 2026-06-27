# include "singleton.h"

namespace 
{
    constexpr int MaxFinalizersSize = 256;
    std::mutex g_Mutex;
    int g_NumFinalizersSize = 0;
    SingletonFinalizer::FinalizerFunc gFinalizers[MaxFinalizersSize];

}

void SingletonFinalizer::AddFinalizer(FinalizerFunc func)
{
   std::lock_guard<std::mutex> lock(g_Mutex);
    gFinalizers[g_NumFinalizersSize++] = func;
}

void SingletonFinalizer::Finalize() 
{
    std::lock_guard<std::mutex> lock(g_Mutex);
    for (int i = g_NumFinalizersSize - 1; i >= 0; --i) 
    {
        (*gFinalizers[i])();
    }
    g_NumFinalizersSize = 0;
}