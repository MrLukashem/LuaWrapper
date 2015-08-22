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

    constexpr int ONE_ELEMENT = 1;

    template <typename T>
    void push_on_stack(lua_State* lua_state, T number) {
    }

    template <>
    void push_on_stack<double>(lua_State* lua_state, double number_to_push) {
        lua_pushnumber(lua_state, number_to_push);
    }

    template <>
    void push_on_stack<float>(lua_State* lua_state, float number_to_push) {
        auto obj_to_push = static_cast<lua_Number>(number_to_push);
        lua_pushnumber(lua_state, obj_to_push);
    }

    template <>
    void push_on_stack<int>(lua_State* lua_state, int number_to_push) {
        lua_pushinteger(lua_state, number_to_push);
    }

    template <>
    void push_on_stack<std::string>(lua_State* lua_state, std::string string_to_push) {
        lua_pushstring(lua_state, string_to_push.c_str());
    }


    template <typename ResultType>
    ResultType get_result_from_lua(lua_State* lua_state) {
        auto pointer_to_result =
                static_cast<ResultType>(lua_touserdata(lua_state, -1));

        return *pointer_to_result;
    }

    template <>
    int get_result_from_lua<int>(lua_State* lua_state) {
        return static_cast<int>(lua_tointeger(lua_state, -1));
    }

    template <>
    std::string get_result_from_lua<std::string>(lua_State* lua_state) {
        return static_cast<std::string>(lua_tostring(lua_state, -1));
    }

    template <>
    double get_result_from_lua<double>(lua_State* lua_state) {
        return static_cast<double>(lua_tonumber(lua_state, -1));
    }

    template <>
    float get_result_from_lua<float>(lua_State* lua_state) {
        return static_cast<float>(lua_tonumber(lua_state, -1));
    }

} // ns

class LuaWrapper {
public:
    LuaWrapper();
    ~LuaWrapper();

    template <typename T>
    auto pushObjectOnLuaStack(T);

    template <typename FirstType>
    void pushObjectsOnLuaStack(FirstType);

    template <typename FirstType, typename... Args>
    void pushObjectsOnLuaStack(FirstType, Args...);

    template <typename ResultType>
    ResultType getResult();

    template <typename ResultType, typename... Args>
    ResultType callLuaFunction(std::string, Args...);

    template <typename ResultType, typename... Args>
    auto getLuaFunction(std::string, Args...);

    static LuaWrapperSptr newInstanceOnStack();

private:
    lua_State* luaState_;
};

template <typename T>
auto LuaWrapper::pushObjectOnLuaStack(T obj_to_push) {
    if(luaState_ != nullptr) {
        push_on_stack<T>(luaState_, obj_to_push);
    }
}


/*
 * A function pushing arguments on a lua stack.
 * args: Variadic template types.
 *
 * return: Void.
 *
 */
template <typename FirstType>
void LuaWrapper::pushObjectsOnLuaStack(FirstType last_arg) {
    pushObjectOnLuaStack<FirstType>(last_arg);
}

template <typename FirstType, typename... Args>
void LuaWrapper::pushObjectsOnLuaStack(FirstType first_arg, Args... args) {
    pushObjectOnLuaStack<FirstType>(first_arg);

    pushObjectsOnLuaStack(args...);
}

/*
 * A function returns result of a lua function.
 *
 * result: result of a lua function.
 */
template <typename ResultType>
ResultType LuaWrapper::getResult() {
    return get_result_from_lua<ResultType>(luaState_);
}

/*
 * Calling function.
 *
 * result: Result of a lua function.
 */
//TODO: FirstType = std::string only?
template <typename ResultType, typename... Args>
ResultType LuaWrapper::callLuaFunction(std::string name, Args... args) {
    //pushing name of a lua function
    lua_getglobal(luaState_, name.c_str());

    //pushing arguments of a lua function
    pushObjectsOnLuaStack(args...);

    //getting result from lua
    auto result = getResult<ResultType>();

    return result;
}

/*
 * A Function returns lambda expression.
 *
 * result: lambda expression.
 */
template <typename ResultType, typename... Args>
auto LuaWrapper::getLuaFunction(std::string name, Args... args) {
    auto result_lambda = [=]() -> auto {
        return this->callLuaFunction<ResultType>(name, args...);
    };

    return result_lambda;
}

} //namespace lua

#endif // LUAWRAPPER_H
