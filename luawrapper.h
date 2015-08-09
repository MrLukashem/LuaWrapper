#ifndef LUAWRAPPER_H
#define LUAWRAPPER_H

#include <lua.hpp>
#include <string>
#include <iostream>
#include <memory>

namespace lua {

class LuaWrapper;
using LuaWrapperSptr = std::shared_ptr<LuaWrapper>;
using LuaWrapperUptr = std::unique_ptr<LuaWrapper>;

namespace {

    template <typename T>
    void pushOnStack(lua_State* lua_state, T number) {
    }

    template <>
    void pushOnStack<double>(lua_State* lua_state, double number_to_push) {
        lua_pushnumber(lua_state, number_to_push);
    }

    template <>
    void pushOnStack<float>(lua_State* lua_state, float number_to_push) {
        auto obj_to_push = static_cast<lua_Number>(number_to_push);
        lua_pushnumber(lua_state, obj_to_push);
    }

    template <>
    void pushOnStack<int>(lua_State* lua_state, int number_to_push) {
        lua_pushinteger(lua_state, number_to_push);
    }

    template <>
    void pushOnStack<std::string>(lua_State* lua_state, std::string string_to_push) {
        lua_pushstring(lua_state, string_to_push.c_str());
    }

} // ns

class LuaWrapper {
public:
    LuaWrapper();
    ~LuaWrapper();

    template <typename T>
    auto pushObjectOnLuaStack(T);

    template <typename FirstType, typename... Args>
    void callLuaFunction(FirstType, Args...);

    template <typename LastType>
    void callLuaFunction(LastType);

    void setFunctionName(const std::string&);

    void setFunctionName(const char*);

    static LuaWrapperSptr newInstanceOnStack();

private:
    lua_State* luaState_;
    bool firstFCall_ = true;
};

template <typename T>
auto LuaWrapper::pushObjectOnLuaStack(T obj_to_push) {
    if(luaState_ != nullptr) {
        pushOnStack<T>(luaState_, obj_to_push);
    }
}

template <typename FirstType, typename... Args>
void LuaWrapper::callLuaFunction(FirstType first_arg, Args... args) {
    if(firstFCall_) { //first arg is function name
        //only for std::string
        lua_getglobal(luaState_, first_arg.c_str());
    } else { //arguments of a function
        pushObjectOnLuaStack<FirstType>(first_arg);
    }
}

template <typename LastType>
void LuaWrapper::callLuaFunction(LastType last_type) {
    //pushing last argument of a lua function
    pushObjectOnLuaStack<LastType>(last_type);
}


} //namespace lua

#endif // LUAWRAPPER_H
