#include <gtest/gtest.h>

#include "InetAddress.h"
#include "Logger.h"

using namespace Lux;
using namespace Lux::Polaris;

TEST(InetAddress, Test) {
    InetAddress addr0(1234);
    ASSERT_EQ(addr0.toIp(), string("0.0.0.0"));
    ASSERT_EQ(addr0.toIpPort(), string("0.0.0.0:1234"));
    ASSERT_EQ(addr0.toPort(), 1234);

    InetAddress addr1(4321, true);
    ASSERT_EQ(addr1.toIp(), string("127.0.0.1"));
    ASSERT_EQ(addr1.toIpPort(), string("127.0.0.1:4321"));
    ASSERT_EQ(addr1.toPort(), 4321);

    InetAddress addr2("1.2.3.4", 8888);
    ASSERT_EQ(addr2.toIp(), string("1.2.3.4"));
    ASSERT_EQ(addr2.toIpPort(), string("1.2.3.4:8888"));
    ASSERT_EQ(addr2.toPort(), 8888);

    InetAddress addr3("255.254.253.252", 65535);
    ASSERT_EQ(addr3.toIp(), string("255.254.253.252"));
    ASSERT_EQ(addr3.toIpPort(), string("255.254.253.252:65535"));
    ASSERT_EQ(addr3.toPort(), 65535);
}


TEST(InetAddress, ResolveTest) {
    InetAddress addr(80);
    if (InetAddress::resolve("google.com", &addr)) {
        LOG_INFO << "google.com resolved to " << addr.toIpPort();
    } else {
        LOG_ERROR << "Unable to resolve google.com";
    }
}


int
main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
