#include "common.hpp"
#include "r128.h"

TEST(arithmethic_int, addition)
{
    using P = R128;

    EXPECT_EQ(P(10.5), P(3.5) + 7);
}

TEST(arithmethic_int, subtraction)
{
    using P = R128;

    EXPECT_EQ(P(-3.5), P(3.5) - 7);
}

TEST(arithmethic_int, multiplication)
{
    using P = R128;

    EXPECT_EQ(P(-24.5), P(3.5) * -7);
}

TEST(arithmethic_int, division)
{
    using P = R128;

    EXPECT_EQ(P(3.5 / 7), P(3.5) / 7);
    EXPECT_EQ(P(-3.5 / 7), P(-3.5) / 7);
    EXPECT_EQ(P(3.5 / -7), P(3.5) / -7);
    EXPECT_EQ(P(-3.5 / -7), P(-3.5) / -7);

//#ifndef NDEBUG
//    EXPECT_DEATH(P(1) / 0, "");
//#endif
}

TEST(arithmethic_int, division_range)
{
    using P = R128;

    // These calculation will overflow and produce
    // wrong results without the intermediate type.
    EXPECT_EQ(P(32), P(256) / 8);
}