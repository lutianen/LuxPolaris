#include "Logger.h"
#include <gtest/gtest.h>

#include <iostream>

TEST(LuxPolaris, gest) { ASSERT_TRUE(1 == 1); }


int
main(int argc, char* argv[]) {

    LOG_INFO << "GoogleTest";

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
