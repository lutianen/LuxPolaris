/**
 * @file Connector.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */

#pragma once

#include <functional>
#include <memory>

#include "InetAddress.h"
#include "NonCopyable.h"

namespace Lux {
    namespace Polaris {

        class Channel;
        class EventLoop;

        class Connector : noncopyable,
                          public std::enable_shared_from_this<Connector> {
        public:
            using NewConnectionCallback = std::function<void(int sockfd)>;

        private:
            enum States { kDisconnected, kConnecting, kConnected };
            static const int kMaxRetryDelayMs = 30 * 1000;
            static const int kInitRetryDelayMs = 500;

            EventLoop* loop_;
            InetAddress serverAddr_;
            bool connect_;  // atomic
            States state_;  // FIXME: use atomic variable
            std::unique_ptr<Channel> channel_;
            NewConnectionCallback newConnectionCallback_;
            int retryDelayMs_;

            void setState(States s) { state_ = s; }
            void startInLoop();
            void stopInLoop();
            void connect();
            void connecting(int sockfd);
            void handleWrite();
            void handleError();
            void retry(int sockfd);
            int removeAndResetChannel();
            void resetChannel();

        public:
            Connector(EventLoop* loop, const InetAddress& serverAddr);
            ~Connector();

            void setNewConnectionCallback(const NewConnectionCallback& cb) {
                newConnectionCallback_ = cb;
            }

            void start();    // can be called in any thread
            void restart();  // must be called in loop thread
            void stop();     // can be called in any thread

            const InetAddress& serverAddress() const { return serverAddr_; }
        };
    }  // namespace Polaris
}  // namespace Lux