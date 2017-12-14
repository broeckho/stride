# CMake generated Testfile for 
# Source directory: /Users/broeckho/usr/projects/c++/stride/test/cpp/gtester
# Build directory: /Users/broeckho/usr/projects/c++/stride/cmake-build-release/test/cpp/gtester
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(gtester_default "/Users/broeckho/opt/stride-2/bin/gtester" "--gtest_filter=*default*" "--gtest_output=xml:gtester_default.xml")
set_tests_properties(gtester_default PROPERTIES  WORKING_DIRECTORY "/Users/broeckho/opt/stride-2/tests")
add_test(gtester_all "/Users/broeckho/opt/stride-2/bin/gtester" "--gtest_output=xml:gtester_all.xml")
set_tests_properties(gtester_all PROPERTIES  WORKING_DIRECTORY "/Users/broeckho/opt/stride-2/tests")
