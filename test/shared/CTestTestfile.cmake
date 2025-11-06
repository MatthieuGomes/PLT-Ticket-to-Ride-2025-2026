# CMake generated Testfile for 
# Source directory: /app/test/shared
# Build directory: /app/test/shared
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_shared "/app/test/shared/test_shared")
set_tests_properties(test_shared PROPERTIES  WORKING_DIRECTORY "/app" _BACKTRACE_TRIPLES "/app/test/CMakeLists.txt;77;add_test;/app/test/shared/CMakeLists.txt;2;add_custom_test;/app/test/shared/CMakeLists.txt;0;")
subdirs("state")
subdirs("cardsState")
subdirs("playersState")
subdirs("mapState")
