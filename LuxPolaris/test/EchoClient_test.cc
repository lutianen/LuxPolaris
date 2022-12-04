/**
 * @file EchoClient_test.cc
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-12
 */

#include <unistd.h>

#include <cstdio>
#include <utility>

#include "EventLoop.h"
#include "InetAddress.h"
#include "Logger.h"
#include "TCPClient.h"
#include "Thread.h"

using namespace Lux;
using namespace Lux::Polaris;

int numThreads = 0;
class EchoClient;
std::vector<std::unique_ptr<EchoClient>> clients;
int current = 0;

class EchoClient : noncopyable {
private:
    EventLoop* loop_;
    TCPClient client_;

    void onConnection(const TCPConnectionPtr& conn) {
        LOG_TRACE << conn->localAddress().toIpPort() << " -> "
                  << conn->peerAddress().toIpPort() << " is "
                  << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected()) {
            ++current;
            if (implicit_cast<size_t>(current) < clients.size()) {
                clients[current]->connect();
            }
            LOG_INFO << "*** connected " << current;
        }
        conn->send("world\n");
    }

    void onMessage(const TCPConnectionPtr& conn, Buffer* buf, Timestamp time) {
        string msg(buf->retrieveAllAsString());
        LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at "
                  << time.toString();
        if (msg == "quit\n") {
            conn->send("bye\n");
            conn->shutdown();
        } else if (msg == "shutdown\n") {
            loop_->quit();
        } else {
            conn->send(msg);
        }
    }

public:
    EchoClient(EventLoop* loop, const InetAddress& listenAddr, const string& id)
        : loop_(loop), client_(loop, listenAddr, "EchoClient" + id) {
        client_.setConnectionCallback(
            std::bind(&EchoClient::onConnection, this, _1));
        client_.setMessageCallback(
            std::bind(&EchoClient::onMessage, this, _1, _2, _3));
        // client_.enableRetry();
    }

    void connect() { client_.connect(); }
    // void stop();
};

int main(int argc, char* argv[]) {
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    if (argc > 1) {
        EventLoop loop;
        bool ipv6 = argc > 3;
        InetAddress serverAddr(argv[1], static_cast<uint16_t>(atoi(argv[2])), ipv6);

        int n = 1;
        if (argc > 3) {
            n = atoi(argv[3]);
        }

        clients.reserve(n);
        for (int i = 0; i < n; ++i) {
            char buf[32];
            snprintf(buf, sizeof buf, "%d", i + 1);
            clients.emplace_back(new EchoClient(&loop, serverAddr, buf));
        }

        clients[current]->connect();
        loop.loop();
    } else {
        printf("Usage: %s host_ip port [current#]\n", argv[0]);
    }
}
