/**
 * @file Poller.cc
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */

#include "Poller.h"

#include "Channel.h"
#include "EPollPoller.h"

using namespace Lux;
using namespace Lux::Polaris;

Poller::Poller(EventLoop* loop) : ownerLoop_(loop) {}

Poller::~Poller() = default;

bool Poller::hasChannel(Channel* channel) const {
    assertInLoopThread();
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

/// @brief
/// @param loop
/// @return
Poller* Poller::newDefaultPoller(EventLoop* loop) {
    if (::getenv("LUX_USE_POLL")) {
        // BUG pollpoller
        return new EPollPoller(loop);
        // return new PollPoller(loop);
    } else {
        return new EPollPoller(loop);
    }
}
