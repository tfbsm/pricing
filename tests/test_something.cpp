#include "libtfbsm/model.hpp"
#include <gtest/gtest.h>

TEST(ModelTest, SomeTest) {
    static const int val = 1337;

    tfbsm::Model m{val};

    ASSERT_EQ(m.getTestField(), val);
}