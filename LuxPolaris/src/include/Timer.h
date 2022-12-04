/**
 * @file Timer.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */

#pragma once

#include "Atomic.h"
#include "Callbacks.h"
#include "Timestamp.h"

namespace Lux {
    namespace Polaris {

        /// @brief Internal class for timer event.
        class Timer : noncopyable {
        public:
            Timer(TimerCallback cb, Timestamp when, double interval)
                : callback_(std::move(cb)),
                  expiration_(when),
                  interval_(interval),
                  repeat_(interval > 0.0),
                  sequence_(s_numCreated_.incrementAndGet()) {}

            void run() const { callback_(); }

            inline Timestamp expiration() const { return expiration_; }
            inline bool repeat() const { return repeat_; }
            inline int64_t sequence() const { return sequence_; }

            void restart(Timestamp now);

            static inline int64_t numCreated() { return s_numCreated_.get(); }

        private:
            const TimerCallback callback_;
            Timestamp expiration_;

            const double interval_;
            const bool repeat_;
            const int64_t sequence_;

            static AtomicInt64 s_numCreated_;
        };

    }  // namespace Polaris
}  // namespace Lux
