set(TEST_SRC
        test_lexer.cpp
        test_parser.cpp
        runner.cpp)

add_executable(tests ${TEST_SRC})

target_link_libraries(tests PUBLIC
        c_compiler_lib
        GTest::gtest_main)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

include(GoogleTest)
