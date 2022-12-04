/**
 * @file Sockets.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-11
 */

#ifndef SOCKETS_H
#define SOCKETS_H

#include <arpa/inet.h>

#include <utility>

#include "InetAddress.h"
#include "NonCopyable.h"

// struct tcp_info in <netinet/tcp.h>
struct tcp_info;

namespace Lux {
    namespace Polaris {
        namespace sockets {

            /// @brief Creates a non-blocking socket file descriptor, abort if
            /// any eror.
            /// @return socket file descriptor
            int createNonblockingOrDie(sa_family_t family);

            int connect(int sockfd, const sockaddr* addr);
            void bindOrDie(int sockfd, const sockaddr* addr);
            void listenOrDie(int sockfd);
            int accept(int sockfd, sockaddr_in6* addr);

            ssize_t read(int sockfd, void* buf, size_t count);
            ssize_t readv(int sockfd, const iovec* iov, int iovcnt);
            ssize_t write(int sockfd, const void* buf, size_t count);

            void close(int sockfd);
            void shutdownWrite(int sockfd);

            void toIpPort(char* buf, size_t size, const sockaddr* addr);
            void toIp(char* buf, size_t size, const sockaddr* addr);

            void fromIpPort(const char* ip, uint16_t port, sockaddr_in* addr);
            void fromIpPort(const char* ip, uint16_t port, sockaddr_in6* addr);

            int getSocketError(int sockfd);

            const sockaddr* sockaddr_cast(const sockaddr_in* addr);
            const sockaddr* sockaddr_cast(const sockaddr_in6* addr);
            sockaddr* sockaddr_cast(sockaddr_in6* addr);
            const sockaddr_in* sockaddr_in_cast(const sockaddr* addr);
            const sockaddr_in6* sockaddr_in6_cast(const sockaddr* addr);

            sockaddr_in6 getLocalAddr(int sockfd);
            sockaddr_in6 getPeerAddr(int sockfd);
            bool isSelfConnect(int sockfd);
        }  // namespace sockets

        /**
         * @brief RAII - fileDescriptor, close it in Dtor.
         */
        class Socket : noncopyable {
        private:
            const int sockfd_;

        public:
            explicit Socket(int sockfd) : sockfd_(sockfd) {}
            // TODO Socket(Socket&&) // move constructor in C++11
            Socket(Socket&& that) : sockfd_(std::move(that.sockfd_)) {}
            ~Socket();

            inline int fd() const { return sockfd_; }

            // return true if success.
            bool getTcpInfo(struct tcp_info*) const;
            bool getTcpInfoString(char* buf, int len) const;

            /// abort if address in use
            void bindAddress(const InetAddress& localaddr);

            /// abort if address in use
            void listen();

            /// On success, returns a non-negative integer that is
            /// a descriptor for the accepted socket, which has been
            /// set to non-blocking and close-on-exec. *peeraddr is assigned.
            /// On error, -1 is returned, and *peeraddr is untouched.
            int accept(InetAddress* peeraddr);

            void shutdownWrite();

            /// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
            void setTcpNoDelay(bool on);

            /// Enable/disable SO_REUSEADDR
            void setReuseAddr(bool on);

            /// Enable/disable SO_REUSEPORT
            void setReusePort(bool on);

            /// Enable/disable SO_KEEPALIVE
            void setKeepAlive(bool on);
        };
    }  // namespace Polaris
}  // namespace Lux

#endif  // SOCKETS_H
