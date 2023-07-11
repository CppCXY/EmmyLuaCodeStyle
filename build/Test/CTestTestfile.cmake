# CMake generated Testfile for 
# Source directory: C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test
# Build directory: C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/build/Test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(TEST "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/build/Test/Debug/CodeFormatTest.exe" "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/test_script/")
  set_tests_properties(TEST PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/CMakeLists.txt;40;add_test;C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(TEST "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/build/Test/Release/CodeFormatTest.exe" "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/test_script/")
  set_tests_properties(TEST PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/CMakeLists.txt;40;add_test;C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(TEST "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/build/Test/MinSizeRel/CodeFormatTest.exe" "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/test_script/")
  set_tests_properties(TEST PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/CMakeLists.txt;40;add_test;C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(TEST "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/build/Test/RelWithDebInfo/CodeFormatTest.exe" "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/test_script/")
  set_tests_properties(TEST PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/CMakeLists.txt;40;add_test;C:/Users/walchshofer/Documents/git/EmmyLuaCodeStyle/Test/CMakeLists.txt;0;")
else()
  add_test(TEST NOT_AVAILABLE)
endif()
