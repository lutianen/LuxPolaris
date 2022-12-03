#include <gtest/gtest.h>

#include "Logger.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Types.h"

#include <sys/timerfd.h>


static int cnt = 20;
void
print(Lux::Polaris::EventLoop* loop, const char* msg) {
    using Lux::Timestamp;

    static std::map<const char*, Timestamp> lasts;
    Timestamp& last = lasts[msg];
    Timestamp now = Timestamp::now();
    printf("%s tid %d %s delay %f\n",
           now.toString().c_str(),
           Lux::CurrentThread::tid(),
           msg,
           timeDifference(now, last));
    last = now;
    cnt--;

    if (cnt < 0)
        loop->quit();
}

namespace Lux::Polaris::detail {
int
createTimerfd();
void
readTimerfd(int timerfd, Timestamp now);
} // namespace Lux::Polaris::detail

// Use relative time, immunized to wall clock changes.
class PeriodicTimer {
public:
    PeriodicTimer(Lux::Polaris::EventLoop* loop, double interval,
                  const Lux::Polaris::TimerCallback& cb)
        : loop_(loop),
          timerfd_(Lux::Polaris::detail::createTimerfd()),
          timerfdChannel_(loop, timerfd_),
          interval_(interval),
          cb_(cb) {
        timerfdChannel_.setReadCallback(
            std::bind(&PeriodicTimer::handleRead, this));
        timerfdChannel_.enableReading();
    }

    void
    start() {
        struct itimerspec spec;
        Lux::memZero(&spec, sizeof spec);
        spec.it_interval = toTimeSpec(interval_);
        spec.it_value = spec.it_interval;
        int ret =
            ::timerfd_settime(timerfd_, 0 /* relative timer */, &spec, NULL);
        if (ret) {
            LOG_SYSERR << "timerfd_settime()";
        }
    }

    ~PeriodicTimer() {
        timerfdChannel_.disableAll();
        timerfdChannel_.remove();
        ::close(timerfd_);
    }

private:
    void
    handleRead() {
        loop_->assertInLoopThread();
        Lux::Polaris::detail::readTimerfd(timerfd_, Lux::Timestamp::now());
        if (cb_)
            cb_();
    }

    static struct timespec
    toTimeSpec(double seconds) {
        struct timespec ts;
        Lux::memZero(&ts, sizeof ts);
        const int64_t kNanoSecondsPerSecond = 1000000000;
        const int kMinInterval = 100000;
        int64_t nanoseconds =
            static_cast<int64_t>(seconds * kNanoSecondsPerSecond);
        if (nanoseconds < kMinInterval)
            nanoseconds = kMinInterval;
        ts.tv_sec = static_cast<time_t>(nanoseconds / kNanoSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(nanoseconds % kNanoSecondsPerSecond);
        return ts;
    }

    Lux::Polaris::EventLoop* loop_;
    const int timerfd_;
    Lux::Polaris::Channel timerfdChannel_;
    const double interval_; // in seconds
    Lux::Polaris::TimerCallback cb_;
};

TEST(Channel, UnitTest) {
    LOG_INFO << "pid = " << getpid() << ", tid = " << Lux::CurrentThread::tid()
             << " Try adjusting the wall clock, see what happens.";
    Lux::Polaris::EventLoop loop;
    PeriodicTimer timer(&loop, 1, std::bind(print, &loop, "PeriodicTimer"));
    timer.start();
    loop.runEvery(3, std::bind(print, &loop, "EventLoop::runEvery"));
    loop.loop();
}


int
main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
