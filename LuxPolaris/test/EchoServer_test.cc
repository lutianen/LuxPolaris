#include "Buffer.h"
#include "Callbacks.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Logger.h"
#include "LuxPolaris.h"
#include "TCPServer.h"
#include "Timestamp.h"

#include <functional>
#include <utility>
#include <cstdio>
#include <unistd.h>

using namespace Lux;
using namespace Lux::Polaris;

int numThreads = 0;

class EchoServer {
private:
    EventLoop* loop_;
    TCPServer server_;

    void
    onConnection(const TCPConnectionPtr& conn) {
        LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
                  << conn->localAddress().toIpPort() << " is "
                  << (conn->connected() ? "UP" : "DOWN");
        LOG_INFO << conn->getTcpInfoString();

        conn->send("hello\n");
    }

    void
    onMessage(const TCPConnectionPtr& conn, Buffer* buf, Timestamp time) {
        string msg(buf->retrieveAllAsString());
        LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at "
                  << time.toString();
        if (msg == "exit\n") {
            conn->send("bye\n");
            conn->shutdown();
        }
        if (msg == "quit\n") {
            loop_->quit();
        }
        conn->send(msg);
    }

public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr)
        : loop_(loop), server_(loop, listenAddr, "LuxPolaris EchoServer") {
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, _1));
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, _1, _2, _3));
        server_.setThreadNum(numThreads);
    }

    void
    start() {
        server_.start();
    }
};


int
main(int argc, char* argv[]) {
    Logger::setLogLevel(Logger::LogLevel::INFO);

    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    LOG_INFO << "sizeof TCPConnection = " << sizeof(TCPConnection);

    if (argc > 1) {
        numThreads = atoi(argv[1]);
    }

    bool ipv6 = argc > 2;

    EventLoop loop;
    InetAddress listenAddr(2022, false, ipv6);
    EchoServer server(&loop, listenAddr);

    server.start();

    loop.loop();
}
