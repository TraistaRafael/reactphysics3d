#include "common.hpp"
#include <fpm/math.hpp>
#include "r128.h"

TEST(trigonometry, sin)
{
    using P = R128;
    const double PI = std::acos(-1);

    constexpr auto MAX_ERROR_PERC = 0.002;

    for (int angle = -1799; angle <= 1800; ++angle)
    {
        auto flt_angle = angle * PI / 180;
        auto sin_real = std::sin(flt_angle);
        auto sin_fixed = static_cast<R128>(std::sin(P(flt_angle)));
        EXPECT_TRUE(HasMaximumError(sin_fixed, sin_real, MAX_ERROR_PERC));
    }
}

TEST(trigonometry, cos)
{
    using P = R128;
    const double PI = std::acos(-1);

    constexpr auto MAX_ERROR_PERC = 0.002;

    for (int angle = -1799; angle <= 1800; ++angle)
    {
        auto flt_angle = angle * PI / 180;
        auto cos_real = std::cos(flt_angle);
        auto cos_fixed = static_cast<double>(std::cos(P(flt_angle)));
        EXPECT_TRUE(HasMaximumError(cos_fixed, cos_real, MAX_ERROR_PERC));
    }
}

//TEST(trigonometry, tan)
//{
//    using P = R128;
//    const double PI = std::acos(-1);
//
//    constexpr auto MAX_ERROR_PERC = 0.002;
//
//    for (int angle = -1799; angle <= 1800; ++angle)
//    {
//        // Tangent goes to infinite at 90 and -90 degrees.
//        // We can't represent that with fixed-point maths, so don't test for it.
//        if ((angle + 90) % 180 != 0)
//        {
//            auto flt_angle = angle * PI / 180;
//            auto tan_real = std::tan(flt_angle);
//            auto tan_fixed = static_cast<double>(std::tan(P(flt_angle)));
//            EXPECT_TRUE(HasMaximumError(tan_fixed, tan_real, MAX_ERROR_PERC));
//        }
//    }
////
////#ifndef NDEBUG
////    EXPECT_DEATH(tan(P::pi()/2), "");
////    EXPECT_DEATH(tan(-P::pi()/2), "");
////#endif
//}
//
//TEST(trigonometry, atan)
//{
//    using P = R128;
//
//    constexpr auto MAX_ERROR_PERC = 0.025;
//
//    for (int x = -1000; x <= 1000; ++x)
//    {
//        auto value = x / 10.0;
//        auto atan_real = std::atan(value);
//        auto atan_fixed = static_cast<double>(std::atan(P(value)));
//        EXPECT_TRUE(HasMaximumError(atan_fixed, atan_real, MAX_ERROR_PERC));
//    }
//
//    for (int x = -1000; x <= 1000; ++x)
//    {
//        auto value = x / 1000.0;
//        auto atan_real = std::atan(value);
//        auto atan_fixed = static_cast<double>(atan(P(value)));
//        EXPECT_TRUE(HasMaximumError(atan_fixed, atan_real, MAX_ERROR_PERC));
//    }
//}
//
//TEST(trigonometry, asin)
//{
//    using P = double;
//
//    constexpr auto MAX_ERROR_PERC = 0.025;
//
//    for (int x = -1000; x <= 1000; ++x)
//    {
//        auto value = x / 1000.0;
//        auto asin_real = std::asin(value);
//        auto asin_fixed = static_cast<double>(asin(P(value)));
//        EXPECT_TRUE(HasMaximumError(asin_fixed, asin_real, MAX_ERROR_PERC));
//    }
//}
//
//TEST(trigonometry, acos)
//{
//    using P = double;
//
//    constexpr auto MAX_ERROR_PERC = 0.025;
//
//    for (int x = -1000; x <= 1000; ++x)
//    {
//        auto value = x / 1000.0;
//        auto acos_real = std::acos(value);
//        auto acos_fixed = static_cast<double>(acos(P(value)));
//        EXPECT_TRUE(HasMaximumError(acos_fixed, acos_real, MAX_ERROR_PERC));
//    }
//}
//
//TEST(trigonometry, atan2)
//{
//    using P = double;
//    const double PI = std::acos(-1);
//
//    constexpr auto MAX_ERROR_PERC = 0.025;
//
//    for (int angle = -1799; angle <= 1800; ++angle)
//    {
//        const auto y = std::sin(angle * PI / 1800);
//        const auto x = std::cos(angle * PI / 1800);
//
//        auto atan2_real = std::atan2(y, x);
//        auto atan2_fixed = static_cast<double>(atan2(P(y), P(x)));
//        EXPECT_TRUE(HasMaximumError(atan2_fixed, atan2_real, MAX_ERROR_PERC));
//    }
//
//#ifndef NDEBUG
//    EXPECT_DEATH(atan2(P(0), P(0)), "");
//#endif
//}

// Naively, atan2(y, x) does y / x which would overflow for near-zero x with Q16.16.
// Test that we've got protections in place for this.
//TEST(trigonometry, atan2_near_zero)
//{
//    constexpr auto MAX_ERROR_PERC = 0.025;
//    using P = double;
//
//    const auto x = P::from_raw_value(1);
//    const auto y = P(100);
//
//    // Positive x
//    {
//        auto atan2_real = std::atan2(static_cast<double>(y), static_cast<double>(x));
//        auto atan2_fixed = static_cast<double>(atan2(y, x));
//        EXPECT_TRUE(HasMaximumError(atan2_fixed, atan2_real, MAX_ERROR_PERC));
//    }
//
//    // Negative x
//    {
//        auto atan2_real = std::atan2(static_cast<double>(y), static_cast<double>(-x));
//        auto atan2_fixed = static_cast<double>(atan2(y, -x));
//        EXPECT_TRUE(HasMaximumError(atan2_fixed, atan2_real, MAX_ERROR_PERC));
//    }
//}
