/**
 * @file Buffer.cc
 * @brief 
 * 
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-11
 */


#include "Buffer.h"
#include "Sockets.h"

#include <errno.h>
#include <sys/uio.h>


using namespace Lux;
using namespace Lux::Polaris;

const char Buffer::kCRLF[] = "\r\n";

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;


//FIXME ???
/// @brief
/// @param fd 
/// @param savedErrno 
/// @return 
ssize_t
Buffer::readFd(int fd, int* savedErrno) {
    // saved an ioctl()/FIONREAD call to tell how much to read
    char extrabuf[65536];

    struct iovec vec[2];
    const size_t writeable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writeable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    /// When there is enough space in this buff, don't read into extrabuf
    /// When extrabuf is used, we read 128k-1 bytes at most. 
    const int iovcnt = (writeable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = sockets::readv(fd, vec, iovcnt);
    if (n < 0) {
        *savedErrno = errno;
    } else if (implicit_cast<size_t>(n) <= writeable) {
        writerIndex_ += static_cast<size_t>(n);
    } else {
        writerIndex_ = buffer_.size();
        append(extrabuf, static_cast<size_t>(n) - writeable);
    }

    return n;    
}

