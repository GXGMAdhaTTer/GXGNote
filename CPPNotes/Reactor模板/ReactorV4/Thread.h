#ifndef __THREAD_H__
#define __THREAD_H__


#include "NonCopyable.h"
#include <pthread.h>
#include <functional>

using std::function;
using std::bind;

using ThreadCallback = function<void()>;

class Thread 
: NonCopyable {
public:
    Thread(ThreadCallback&& cb);
    ~Thread();
    void start();
    void join();

private:
    static void* threadFunc(void* arg);
private:
    pthread_t _thid;
    bool _isRunning;
    ThreadCallback _cb;
};

#endif

