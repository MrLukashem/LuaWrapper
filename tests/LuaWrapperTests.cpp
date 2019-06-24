//
// Created by MrLukashem on 24.03.2019.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "../libs/catch.hpp"

#include "../LuaWrapper.h"


const std::string LUA_FILE_PATH =
        R"(C:\Users\lmerta\Documents\LuaWrapper_2019\LuaWrapper_2019\tests\lua_functions.lua)";


TEST_CASE( "add function", "[LuaWrapper]" ) {
    auto luaWrapper = lua::LuaWrapper(LUA_FILE_PATH);

    SECTION( "add function: 2 + 3" ) {
        constexpr int validResult = 5;
        constexpr int firstArg = 2;
        constexpr int secondArg = 3;

        const auto result = luaWrapper.call<int>("add", firstArg, secondArg);
        REQUIRE(result == validResult);
    }

    SECTION( "add function: 10 + 12" ) {
        constexpr int validResult = 22;
        constexpr int firstArg = 10;
        constexpr int secondArg = 12;

        const auto result = luaWrapper.call<int>("add", firstArg, secondArg);
        REQUIRE(result == validResult);
    }

    SECTION( "add function: 32 + (-2)" ) {
        constexpr int validResult = 30;
        constexpr int firstArg = 32;
        constexpr int secondArg = -2;

        const auto result = luaWrapper.call<int>("add", firstArg, secondArg);
        REQUIRE(result == validResult);
    }
}

TEST_CASE( "multiple function", "[LuaWrapper]" ) {
    auto luaWrapper = lua::LuaWrapper(LUA_FILE_PATH);

    SECTION( "multiple function: 2 * 3" ) {
        constexpr int validResult = 6;
        constexpr int firstArg = 2;
        constexpr int secondArg = 3;

        const auto result = luaWrapper.call<int>("multiple", firstArg, secondArg);
        REQUIRE(result == validResult);

        const auto fun = luaWrapper.grabAndBoundArgs<int>("multiple", firstArg, secondArg);
        REQUIRE(fun() == validResult);

        const auto fun2 = luaWrapper.grab<int, int, int>("multiple");
        REQUIRE(fun2(firstArg, secondArg) == validResult);
    }
}

TEST_CASE( "getArray functions", "[LuaWrapper]" ) {
    auto luaWrapper = lua::LuaWrapper(LUA_FILE_PATH);

    SECTION( "getArray function: {111, 222, 333}" ) {
        const std::vector<int> validResult {111, 222, 333};

        const auto result = luaWrapper.call<std::vector<int>>("getArray");
        REQUIRE(validResult == result);
    }

    SECTION( "getTransformedArray function: {111 * 7, 222 * 7, 333 * 7}" ) {
        const std::vector<int> validResult {111 * 7, 222 * 7, 333 * 7};

        const auto result = luaWrapper.call<std::vector<int>>("getTransformedArray");
        REQUIRE(validResult == result);
    }
}