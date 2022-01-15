#include "common.hpp"
#include "r128.h"

TEST(arithmethic, negation)
{
    using P = R128;

    EXPECT_EQ(P(-13.125), -P( 13.125));
    EXPECT_EQ(P( 13.125), -P(-13.125));
}

TEST(arithmethic, addition)
{
    using P = R128;

    EXPECT_EQ(P(10.75), P(3.5) + P(7.25));
}

TEST(arithmethic, subtraction)
{
    using P = R128;

    EXPECT_EQ(P(-3.75), P(3.5) - P(7.25));
}

TEST(arithmethic, multiplication)
{
    using P = R128;

    EXPECT_EQ(P(-25.375), P(3.5) * P(-7.25));
}

TEST(arithmethic, division)
{
    using P = R128;

    EXPECT_EQ(P(3.5 / 7.25), P(3.5) / P(7.25));
    EXPECT_EQ(P(-3.5 / 7.25), P(-3.5) / P(7.25));
    EXPECT_EQ(P(3.5 / -7.25), P(3.5) / P(-7.25));
    EXPECT_EQ(P(-3.5 / -7.25), P(-3.5) / P(-7.25));

//#ifndef NDEBUG
//    EXPECT_DEATH(P(1) / P(0), "");
//#endif
}

TEST(arithmethic, division_range)
{
    using P = R128;

    // These calculation will overflow and produce
    // wrong results without the intermediate type.
    EXPECT_EQ(P(32), P(256) / P(8));
}