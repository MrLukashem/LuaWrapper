cmake_minimum_required(VERSION 3.12)
project(LuaWrapper_2019)

set(CMAKE_CXX_STANDARD 17)

add_executable(LuaWrapper_2019 LuaWrapper.cpp LuaWrapper.h tests/LuaWrapperTests.cpp)
target_link_libraries(LuaWrapper_2019 ${CMAKE_SOURCE_DIR}/libs/lua-5.3.5_Win32_mingw6_lib/liblua53.a)