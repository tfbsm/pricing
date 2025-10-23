#include <gtest/gtest.h>

#include "libtfbsm/model.hpp"

TEST(ModelTest, SomeTest) {
    static int const val = 1337;

    tfbsm::Model m{val};

    ASSERT_EQ(m.getTestField(), val);
}