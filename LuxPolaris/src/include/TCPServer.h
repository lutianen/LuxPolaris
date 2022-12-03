/**
 * @file TCPServer.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */


#pragma once

#include "Atomic.h"
#include "TCPConnection.h"

#include <map>


namespace Lux {
namespace Polaris {

    class Acceptor;
    class EventLoop;
    class EventLoopThreadPool;


    /// TCP server, supports single-threaded and thread-pool models.
    /// This is an interface class, so don't expose too much details.
    class TCPServer : noncopyable {
    public:
        using ThreadInitCallback = std::function<void(EventLoop*)>;
        enum class Option { kNoReusePort, kReusePort };

    private:
        using ConnectionMap = std::map<std::string, TCPConnectionPtr>;

        EventLoop* loop_; // the acceptor loop
        const std::string ipPort_;
        const std::string name_;
        std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
        std::shared_ptr<EventLoopThreadPool> threadPool_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        ThreadInitCallback threadInitCallback_;
        AtomicInt32 started_;
        // always in loop thread
        int nextConnId_;
        ConnectionMap connections_;

    private:
        /// Not thread safe, but in loop
        void
        newConnection(int sockfd, const InetAddress& peerAddr);
        /// Thread safe.
        void
        removeConnection(const TCPConnectionPtr& conn);
        /// Not thread safe, but in loop
        void
        removeConnectionInLoop(const TCPConnectionPtr& conn);

    public:
        // TcpServer(EventLoop* loop, const InetAddress& listenAddr);
        TCPServer(EventLoop* loop, const InetAddress& listenAddr,
                  const std::string& nameArg,
                  Option option = Option::kNoReusePort);
        ~TCPServer(); // force out-line dtor, for std::unique_ptr members.

        inline const std::string&
        ipPort() const {
            return ipPort_;
        }
        inline const std::string&
        name() const {
            return name_;
        }
        inline EventLoop*
        getLoop() const {
            return loop_;
        }

        /// Set the number of threads for handling input.
        ///
        /// Always accepts new connection in loop's thread.
        /// Must be called before @c start
        /// @param numThreads
        /// - 0 means all I/O in loop's thread, no thread will created. this is
        /// the default value.
        /// - 1 means all I/O in another thread.
        /// - N means a thread pool with N threads, new connections are assigned
        /// on a round-robin basis.
        void
        setThreadNum(int numThreads);

        inline void
        setThreadInitCallback(const ThreadInitCallback& cb) {
            threadInitCallback_ = cb;
        }
        /// valid after calling start()
        inline std::shared_ptr<EventLoopThreadPool>
        threadPool() {
            return threadPool_;
        }

        /// Starts the server if it's not listenning.
        ///
        /// It's harmless to call it multiple times.
        /// Thread safe.
        void
        start();

        /// Set connection callback.
        /// Not thread safe.
        inline void
        setConnectionCallback(const ConnectionCallback& cb) {
            connectionCallback_ = cb;
        }

        /// Set message callback.
        /// Not thread safe.
        inline void
        setMessageCallback(const MessageCallback& cb) {
            messageCallback_ = cb;
        }

        /// Set write complete callback.
        /// Not thread safe.
        inline void
        setWriteCompleteCallback(const WriteCompleteCallback& cb) {
            writeCompleteCallback_ = cb;
        }
    };
} // namespace Polaris
} // namespace Lux
