/**
 * @file Callbacks.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-11
 */

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "Timestamp.h"

#include <functional>
#include <memory>

namespace Lux {
template <typename T>
inline T*
get_pointer(const std::shared_ptr<T>& ptr) {
    return ptr.get();
}

template <typename T>
inline T*
get_pointer(const std::unique_ptr<T>& ptr) {
    return ptr.get();
}

// Adapted from google-protobuf stubs/common.h
// see License in muduo/base/Types.h
template <typename To, typename From>
inline ::std::shared_ptr<To>
down_pointer_cast(const ::std::shared_ptr<From>& f) {
    if (false) {
        implicit_cast<From*, To*>(0);
    }

#ifndef NDEBUG
    assert(f == NULL || dynamic_cast<To*>(get_pointer(f)) != NULL);
#endif
    return ::std::static_pointer_cast<To>(f);
}

namespace Polaris {
    // All client visible callbacks go here.

    class Buffer;
    class TCPConnection;
    typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;
    typedef std::function<void()> TimerCallback;
    typedef std::function<void(const TCPConnectionPtr&)> ConnectionCallback;
    typedef std::function<void(const TCPConnectionPtr&)> CloseCallback;
    typedef std::function<void(const TCPConnectionPtr&)> WriteCompleteCallback;
    typedef std::function<void(const TCPConnectionPtr&, size_t)> HighWaterMarkCallback;

    // the data has been read to (buf, len)
    typedef std::function<void(const TCPConnectionPtr&, Buffer*, Timestamp)> MessageCallback;

    void
    defaultConnectionCallback(const TCPConnectionPtr& conn);
    void
    defaultMessageCallback(const TCPConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

} // namespace Polaris
} // namespace Lux

#endif // CALLBACKS_H
