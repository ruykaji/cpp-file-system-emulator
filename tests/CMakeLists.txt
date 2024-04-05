macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} "${ARGV0}.cpp")
    target_link_libraries(${TESTNAME} PRIVATE ${PROJECT_NAME}_lib GTest::gtest)
    target_include_directories(${TESTNAME} PRIVATE ${GTEST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
    
    gtest_discover_tests(${TESTNAME})
endmacro()

package_add_test(file_system_emulator)