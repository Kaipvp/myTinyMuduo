#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace CurrentThread
{
    // 保存tid缓存 因为系统调用非常耗时 拿到tid后将其保存
    extern __thread int t_cachedTid = 0;
    void cachedTid()
    {
        if(t_cachedTid == 0)
        {
            t_cachedTid = static_cast<pid_t>(syscall(SYS_gettid));
        }
    }
    
    inline int tid()
    {
        if(__builtin_expect(t_cachedTid == 0, 0))
        {
            cachedTid();
        }
        return t_cachedTid;
    }
}