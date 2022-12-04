/**
 * @file EventLoopThreadPool.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Logger.h"
#include "NonCopyable.h"
#include "Types.h"

namespace Lux {
    namespace Polaris {

        class EventLoop;
        class EventLoopThread;

        class EventLoopThreadPool : noncopyable {
        public:
            using ThreadInitCallback = std::function<void(EventLoop*)>;

        private:
            EventLoop* baseLoop_;
            string name_;
            bool started_;
            int numThreads_;
            int next_;
            std::vector<std::unique_ptr<EventLoopThread>> threads_;
            std::vector<EventLoop*> loops_;

        public:
            EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg);
            ~EventLoopThreadPool();

            inline void setThreadNum(int numThreads) {
                LOG_DEBUG << "numThreads: " << numThreads_;
                numThreads_ = numThreads;
            }

            void start(const ThreadInitCallback& cb = ThreadInitCallback());

            /// @brief valid after calling start()
            /// round-robin
            /// @return EventLoop*
            EventLoop* getNextLoop();

            /// @brief with the same hash code, it will always return the same
            /// EventLoop
            /// @param hashCOde
            /// @return EventLoop*
            EventLoop* getLoopForHash(size_t hashCOde);

            std::vector<EventLoop*> getAllLoops();

            inline bool started() const { return started_; }

            inline const string& name() const { return name_; }
        };

    }  // namespace Polaris
}  // namespace Lux
