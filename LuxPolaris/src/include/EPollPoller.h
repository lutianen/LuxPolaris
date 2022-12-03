/**
 * @file EPollPoller.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */

#pragma once

#include "Poller.h"
#include <vector>


// typedef union epoll_data {
//   void *ptr;
//   int fd;
//   uint32_t u32;
//   uint64_t u64;
// } epoll_data_t;
// struct epoll_event {
//   uint32_t events;	/* Epoll events */
//   epoll_data_t data;	/* User data variable */
// } __EPOLL_PACKED;
struct epoll_event;


namespace Lux {
namespace Polaris {
    /// IO Multiplexing with epoll(4).
    class EPollPoller : public Poller {
    private:
        using EventList = std::vector<epoll_event>;
        // FIXME InitEventListSize 16
        static const int kInitEventListSize = 16;
        //
        int epollfd_;
        EventList events_;

        static const char*
        operationToString(int op);

        void
        fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
        void
        update(int operation, Channel* channel);

    public:
        EPollPoller(EventLoop* loop);
        ~EPollPoller() override;

        Timestamp
        poll(int timeoutMs, ChannelList* activeChannels) override;
        void
        updateChannel(Channel* channel) override;
        void
        removeChannel(Channel* channel) override;
    };
} // namespace Polaris

} // namespace Lux
