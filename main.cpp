#include <iostream>
#include "luawrapper.h"

int main() {

    auto instance = lua::LuaWrapper::newInstanceOnStack();
    instance->callLuaFunction<int>("qweqw", 3.0f, 2);
    auto f = instance->getLuaFunction<int>("qwewq", 3.2);
    f();

    return 0;
}

