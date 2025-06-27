#include <gtest/gtest.h>
#include <Box.hpp>

TEST(Box, ClipCollide) {
    Box box1(1.f, 1.f, 1.f, 1.f);
    Box box2(3.f, 1.f, 1.f, 1.f);

    ASSERT_EQ(box2.ClipCollideX(box1, 3.f), 1.f);
    box2.SetPosition(1.f, 3.f);

    ASSERT_EQ(box2.ClipCollideY(box1, 3.f), 1.f);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}