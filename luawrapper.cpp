#include "luawrapper.h"

using namespace lua;

LuaWrapper::LuaWrapper() {
    luaState_ = luaL_newstate();
    luaL_openlibs(luaState_);
}

LuaWrapper::~LuaWrapper() {
    lua_close(luaState_);
}

LuaWrapperSptr LuaWrapper::newInstanceOnStack() {
    return std::make_shared<LuaWrapper>();
}
