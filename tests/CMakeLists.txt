cmake_minimum_required(VERSION 3.20)

# Find GoogleTest (or use Catch2)
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "GTest not found, using simple test framework")
    # Create separate executables for each test file
    add_executable(test_schemas core/test_schemas.cpp)
    target_link_libraries(test_schemas PRIVATE rdx_core)
    add_test(NAME TestSchemas COMMAND test_schemas)
    
    add_executable(test_lcm core/test_lcm.cpp)
    target_link_libraries(test_lcm PRIVATE rdx_core)
    add_test(NAME TestLCM COMMAND test_lcm)
    
    add_executable(test_roundtrip core/test_roundtrip.cpp)
    target_link_libraries(test_roundtrip PRIVATE rdx_core)
    add_test(NAME TestRoundtrip COMMAND test_roundtrip)
    
    add_executable(test_rdx_end_to_end integration/test_rdx_end_to_end.cpp)
    target_link_libraries(test_rdx_end_to_end PRIVATE rdx_core)
    add_test(NAME TestRDXEndToEnd COMMAND test_rdx_end_to_end)
else()
    enable_language(CXX)
    
    include(GoogleTest)
    
    add_executable(test_schemas core/test_schemas.cpp)
    target_link_libraries(test_schemas PRIVATE rdx_core GTest::gtest GTest::gtest_main)
    gtest_discover_tests(test_schemas)
    
    add_executable(test_lcm core/test_lcm.cpp)
    target_link_libraries(test_lcm PRIVATE rdx_core GTest::gtest GTest::gtest_main)
    gtest_discover_tests(test_lcm)
    
    add_executable(test_roundtrip core/test_roundtrip.cpp)
    target_link_libraries(test_roundtrip PRIVATE rdx_core GTest::gtest GTest::gtest_main)
    gtest_discover_tests(test_roundtrip)
    
    add_executable(test_rdx_end_to_end integration/test_rdx_end_to_end.cpp)
    target_link_libraries(test_rdx_end_to_end PRIVATE rdx_core GTest::gtest GTest::gtest_main)
    gtest_discover_tests(test_rdx_end_to_end)
endif()

