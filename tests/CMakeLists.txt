add_executable(test_oidx test_oidx.cpp)
target_link_libraries(test_oidx ${PROJECT_NAME})
add_test(NAME oidx-test COMMAND test_oidx)
