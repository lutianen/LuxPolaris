/**
 * @file TimerId.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */

#pragma once

#include <cstdint>

#include "Copyable.h"

namespace Lux {
    namespace Polaris {

        /// Forward declare
        class Timer;

        /// @brief An opaque identifier, for canceling Timer.
        class TimerId : public Lux::copyable {
        public:
            TimerId() : timer_(nullptr), sequence_(0) {}

            TimerId(Timer* timer, int64_t seq)
                : timer_(timer), sequence_(seq) {}

            // default copy-ctor, dtor and assignment are okay

            friend class TimerQueue;

        private:
            Timer* timer_;
            int64_t sequence_;
        };

    }  // namespace Polaris
}  // namespace Lux
