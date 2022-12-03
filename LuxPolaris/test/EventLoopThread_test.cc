#include <functional>
#include <gtest/gtest.h>

#include "CurrentThread.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

using namespace Lux;
using namespace Lux::Polaris;

void
print(EventLoop* p = nullptr) {
    printf("print: pid = %d, tid = %d, loop = %p\n",
           getpid(),
           CurrentThread::tid(),
           p);
}

void quit(EventLoop* p) {
    print(p);
    p->quit();
}

int
main(int argc, char* argv[]) {
    print();

    {
        EventLoopThread thr1;
    }

    {
        EventLoopThread thr2;
        EventLoop* loop = thr2.startLoop();
        loop->runInLoop(std::bind(print, loop));
        CurrentThread::sleepUsec(500 * 1000);
    }

    {
        EventLoopThread thr3;
        EventLoop* loop = thr3.startLoop();
        loop->runInLoop(std::bind(quit, loop));
        CurrentThread::sleepUsec(500 * 1000);
    }
}
