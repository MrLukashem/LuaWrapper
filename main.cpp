#include <iostream>
#include "luawrapper.h"

int main() {

    auto instance = lua::LuaWrapper::newInstanceOnStack();
    instance->pushObjectOnLuaStack<int>(3);

    lua::LuaWrapper wrapper{};

    wrapper.pushObjectOnLuaStack<int>(2);
    std::string e ="qweqw";
    wrapper.callLuaFunction(e, 2, 3, 1, "qweqw", 3.0f);
    return 0;
}

