add_executable(LuxPolarisInitTest LuxPolaris_Init_test.cc)
target_link_libraries(LuxPolarisInitTest LuxLog gtest pthread)

add_executable(LuxAnyTest LuxAny_test.cc)

add_executable(AtomicTest Atomic_test.cc)
target_link_libraries(AtomicTest LuxLog gtest pthread)


add_executable(BufferTest Buffer_test.cc)
target_link_libraries(BufferTest LuxLog LuxPolaris gtest pthread)


add_executable(ChannelTest Channel_test.cc)
target_link_libraries(ChannelTest LuxLog LuxPolaris gtest pthread)

add_executable(EventLoopTest EventLoop_test.cc)
target_link_libraries(EventLoopTest LuxLog LuxPolaris gtest pthread)

add_executable(EventLoopThreadTest EventLoopThread_test.cc)
target_link_libraries(EventLoopThreadTest LuxLog LuxPolaris gtest pthread)

add_executable(EventLoopThreadPoolTest EventLoopThreadPool_test.cc)
target_link_libraries(EventLoopThreadPoolTest LuxLog LuxPolaris gtest pthread)

add_executable(InetAddressTest InetAddress_test.cc)
target_link_libraries(InetAddressTest LuxLog LuxPolaris gtest pthread)

add_executable(TimerQueueTest TimerQueue_test.cc)
target_link_libraries(TimerQueueTest LuxLog LuxPolaris gtest pthread)

add_executable(EchoServerTest EchoServer_test.cc)
target_link_libraries(EchoServerTest LuxLog LuxPolaris gtest pthread)

add_executable(EchoClientTest EchoClient_test.cc)
target_link_libraries(EchoClientTest LuxLog LuxPolaris gtest pthread)
