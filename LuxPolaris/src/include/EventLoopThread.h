/**
 * @file EventLoopThread.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */

#pragma once

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"

namespace Lux {
    namespace Polaris {
        class EventLoop;

        class EventLoopThread : noncopyable {
        public:
            using ThreadInitCallback = std::function<void(EventLoop* loop)>;

        private:
            EventLoop* loop_ GUARDED_BY(mutex_);
            bool exiting_;
            Thread thread_;
            MutexLock mutex_;
            Condition cond_ GUARDED_BY(mutex_);
            ThreadInitCallback callback_;

        private:
            void threadFunc();

        public:
            EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                            const std::string& name = std::string());
            ~EventLoopThread();

            EventLoop* startLoop();
        };
    }  // namespace Polaris
}  // namespace Lux
