include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(
    fpm_test
    arithmetic.cpp
    arithmetic_int.cpp
    basic_math.cpp
    classification.cpp
    CMakeLists.txt
    common.hpp
    constants.cpp
    conversion.cpp
    customizations.cpp
    detail.cpp
    input.cpp
    manip.cpp
    nearest.cpp
    output.cpp
    power.cpp
    trigonometry.cpp
)

target_link_libraries(
    fpm_test
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(fpm_test)