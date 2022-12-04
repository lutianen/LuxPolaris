# LuxPolaris

LuxPolaris (Lux's Polaris) is a event-driven network library in Linux.

## NOTES

- if use c++11, please don't use googletest.
  
  The 1.12.x branch of googletest will be the last to support C++11. Future releases will require at least C++14.
  
  So we suggest that you use c++ >= 14;

## ENVIRONMENTS

- Linux Kernel >= 2.6.25
- C++ >= 11
- CMake >= 3.24

## I/O 复用

  IO复用的基本思想是事件驱动，服务器同时保持多个客户端IO连接，当这个IO上有可读或可写事件发生时，表示这个IO对应的客户端在请求服务器的某项服务，此时服务器响应该服务。在Linux系统中，IO复用使用select, poll和epoll来实现。epoll改进了前两者，更加高效、性能更好，是目前几乎所有高并发服务器的基石。

### Epoll

  epoll监听事件的描述符会放在一颗红黑树上，我们将要监听的IO口放入epoll红黑树中，就可以监听该IO上的事件。

  `epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev)`

  `epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev)`

  `epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, nullptr)`

  ```c++
    typedef union epoll_data {
      void *ptr;
      int fd;
      uint32_t u32;
      uint64_t u64;
    } epoll_data_t;
    struct epoll_event {
      uint32_t events; /* Epoll events */
      epoll_data_t data; /* User data variable */
    } __EPOLL_PACKED;
  ```

  epoll默认采用LT触发模式，即水平触发，只要fd上有事件，就会一直通知内核。这样可以保证所有事件都得到处理、不容易丢失，但可能发生的大量重复通知也会影响epoll的性能。
  
  如使用ET模式，即边缘触法，fd从无事件到有事件的变化会通知内核一次，之后就不会再次通知内核。这种方式十分高效，可以大大提高支持的并发度，但程序逻辑必须一次性很好地处理该fd上的事件，编程比LT更繁琐。
  
  注意ET模式必须搭配非阻塞式socket使用。

  随时使用`epoll_wait`获取有事件发生的fd:

  ```c
    int nfds = epoll_wait(epfd, events, maxevents, timeout);
    
    // 其中events是一个epoll_event结构体数组
    // maxevents是可供返回的最大事件大小，一般是events的大小
    // timeout表示最大等待时间，设置为-1表示一直等待
  ```

### Channel - epoll 高级用法

  ``` c
    typedef union epoll_data {
      void *ptr;
      int fd;
      uint32_t u32;
      uint64_t u64;
    } epoll_data_t;
    struct epoll_event {
      uint32_t events; /* Epoll events */
      epoll_data_t data; /* User data variable */
    } __EPOLL_PACKED;
  ```

  epoll中的 `data` 其实是一个union类型，可以储存一个指针。而通过指针，理论上我们可以指向任何一个地址块的内容，可以是一个类的对象，这样就可以将一个文件描述符封装成一个`Channel`类.
  
  一个`Channel`类自始至终只负责一个文件描述符，对不同的服务、不同的事件类型，都可以在类中进行不同的处理，而不是仅仅拿到一个int类型的文件描述符。

  Core Member of `Channel` class:
  
  ``` c++
    class Channel : noncopyable {
    private:
        EventLoop* loop_;
        const int fd_;
        int events_;
        int revents_; // It is received event types of epoll or poll
        int index_;
        bool logHup_;
        ...
    };
  ```

  事件变量，events表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不一样。revents表示在epoll返回该Channel时文件描述符正在发生的事件。inEpoll表示当前Channel是否已经在epoll红黑树中，为了注册Channel的时候方便区分使用EPOLL_CTL_ADD还是EPOLL_CTL_MOD。

### EventLoop - Reactor

  EventLoop（libevent中叫做EventBase），这是一个事件循环，我们添加需要监听的事务到这个事件循环内，每次有事件发生时就会通知（在程序中返回给我们Channel），然后根据不同的描述符、事件类型进行处理（以回调函数的方式）

  Core Member of `EventLoop` class:
  
  ``` c++
    class EventLoop : noncopyable {
    private:
        bool looping_; /* atomic */
        std::atomic<bool> quit_;
        std::unique_ptr<Poller> poller_;

        // scratch variables
        ChannelList activeChannels_;
        Channel* currentActiveChannel_;
        ...

    public:
        void loop();
        void updateChannel(Channel*):
        ...
    }；
  ```

### timerfd

  `timerfd`是`Linux`为用户程序提供的一个定时器接口。这个接口基于文件描述符，通过文件描述符的可读事件进行超时通知，所以能够被用于`select/poll`的应用场景。`timerfd`是`linux`内核`2.6.25`版本中加入的接口。

  `timerfd`、`eventfd`、`signalfd`配合`epoll`使用，可以构造出一个零轮询的程序，但程序没有处理的事件时，程序是被阻塞的。这样的话在某些移动设备上程序更省电。

  `clock_gettime`函数可以获取系统时钟，精确到纳秒。需要在编译时指定库：`-lrt`。可以获取两种类型事件：

  `CLOCK_REALTIME`：相对时间，从`1970.1.1`到目前的时间。更改系统时间会更改获取的值。也就是，它以系统时间为坐标。

  `CLOCK_MONOTONIC`：与`CLOCK_REALTIME`相反，它是以绝对时间为准，获取的时间为系统重启到现在的时间，更改系统时间对齐没有影响。

  ---

#### `timerfd_create`

生成一个定时器对象，返回与之关联的文件描述符。接收两个入参，一个是`clockid`，填写`CLOCK_REALTIME`或者`CLOCK_MONOTONIC`，参数意义同上。第二个可以传递控制标志：`TFD_NONBLOCK`（非阻塞），`TFD_CLOEXEC`（同`O_CLOEXEC`）

注：`timerfd`的进度要比`usleep`要高。

  ---

#### `timerfd_settime`
  
能够启动和停止定时器；可以设置第二个参数：`flags`，`0`表示是相对定时器，`TFD_TIMER_ABSTIME`表示是绝对定时器。

第三个参数设置超时时间，如果为`0`则表示停止定时器。定时器设置超时方法：

1. 设置超时时间是需要调用`clock_gettime`获取当前时间，如果是绝对定时器，那么需要获取`CLOCK_REALTIME`，在加上要超时的时间。如果是相对定时器，要获取`CLOCK_MONOTONIC`时间。

1. 数据结构：

    ```c
    struct timespec {
      time_t tv_sec;                /* Seconds */
      long   tv_nsec;               /* Nanoseconds */
    };

    struct itimerspec {
      struct timespec it_interval;  /* Interval for periodic timer */
      struct timespec it_value;     /* Initial expiration */
    };
    ```

    `it_value`是首次超时时间，需要填写从`clock_gettime`获取的时间，并加上要超时的时间。 `it_interval`是后续周期性超时时间，是多少时间就填写多少。

    注意一个容易犯错的地方：**`tv_nsec`加上去后一定要判断是否超出`1000000000`（如果超过要秒加一），否则会设置失败。**

    ```c++
    `it_interval`不为`0`则表示是周期性定时器。

    `it_value`和`it_interval`都为`0`表示停止定时器。
    ```

    注：`timerfd_create`第一个参数和`clock_gettime`的第一个参数都是`CLOCK_REALTIME`或者`CLOCK_MONOTONIC`，`timerfd_settime`的第二个参数为`0`（相对定时器）或者`TFD_TIMER_ABSTIME`，三者的关系：

    1. 如果`timerfd_settime`设置为`TFD_TIMER_ABSTIME`（决定时间），则后面的时间必须用`clock_gettime`来获取，获取时设置`CLOCK_REALTIME`还是`CLOCK_MONOTONIC`取决于`timerfd_create`设置的值。
    2. 如果`timerfd_settime`设置为`0`（相对定时器），则后面的时间必须用相对时间，就是：

    ```c
    new_value.it_value.tv_nsec = 500000000;

    new_value.it_value.tv_sec = 3;

    new_value.it_interval.tv_sec = 0;

    new_value.it_interval.tv_nsec = 10000000;
    ```

    > `read`函数可以读`timerfd`，读的内容为`uint_64`，表示超时次数。
