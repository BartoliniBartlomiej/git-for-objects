include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(versioned_tests
    test_version.cpp
)

target_link_libraries(versioned_tests PRIVATE versioned::versioned GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(versioned_tests)