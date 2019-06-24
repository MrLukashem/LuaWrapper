//
// Created by MrLukashem on 22.03.2019.
//

#ifndef LUAWRAPPER_2019_LUAWRAPPER_H
#define LUAWRAPPER_2019_LUAWRAPPER_H


#include "libs/lua-5.3.5_Win32_mingw6_lib/include/lua.hpp"

#include <memory>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace lua {
class LuaWrapper;

using LuaStateDeleter = std::function<void(lua_State *)>;

namespace {
constexpr int STACK_TOP = -1;

template <typename T>
struct is_std_vector : std::false_type {};

template <typename T>
struct is_std_vector<std::vector<T>> : std::true_type {};

template<typename T, typename = typename std::enable_if_t<std::disjunction_v<
        std::is_same<T, int>,
        std::is_same<T, double>,
        std::is_same<T, float>,
        std::is_same<T, bool>,
        std::is_same<T, std::string> >>>
void push_on_stack(lua_State *const lua_state, const T value) {
}

template<>
void push_on_stack<double>(lua_State *const lua_state, const double value) {
    lua_pushnumber(lua_state, value);
}

template<>
void push_on_stack<float>(lua_State *const lua_state, const float value) {
    lua_pushnumber(lua_state, static_cast<lua_Number>(value));
}

template<>
void push_on_stack<int>(lua_State *const lua_state, const int value) {
    lua_pushinteger(lua_state, value);
}

template<>
void push_on_stack<bool>(lua_State *const lua_state, const bool value) {
    lua_pushboolean(lua_state, value);
}

template<>
void push_on_stack<std::string>(lua_State *const lua_state, const std::string value) {
    lua_pushstring(lua_state, value.c_str());
}

template<typename ResultType, typename = typename std::enable_if_t<std::disjunction_v<
        std::is_same<ResultType, int>,
        std::is_same<ResultType, double>,
        std::is_same<ResultType, float>,
        std::is_same<ResultType, bool>,
        std::is_same<ResultType, std::string>,
        is_std_vector<ResultType> >>>
ResultType get_result_from_lua(lua_State *const lua_state) {
    constexpr std::size_t arrayIndex = 1;
    constexpr std::size_t numElementsToPop = 1;

    luaL_checktype(lua_state, arrayIndex, LUA_TTABLE);
    const std::size_t arraySize = luaL_len(lua_state, arrayIndex);

    ResultType container{};
    for (std::size_t i = 1; i <= arraySize; i++) {
        lua_rawgeti(lua_state, arrayIndex, i);
        container.push_back(get_result_from_lua<typename ResultType::value_type>(lua_state));
        lua_pop(lua_state, numElementsToPop);
    }

    return container;
}

template<>
int get_result_from_lua<int>(lua_State *const lua_state) {
    return static_cast<int>(lua_tointeger(lua_state, STACK_TOP));
}

template<>
std::string get_result_from_lua<std::string>(lua_State *const lua_state) {
    return static_cast<std::string>(lua_tostring(lua_state, STACK_TOP));
}

template<>
double get_result_from_lua<double>(lua_State *const lua_state) {
    return static_cast<double>(lua_tonumber(lua_state, STACK_TOP));
}

template<>
float get_result_from_lua<float>(lua_State *const lua_state) {
    return static_cast<float>(lua_tonumber(lua_state, STACK_TOP));
}

} // anonymous namespace

class LuaWrapper {
public:
    LuaWrapper(const std::string& lua_file_source);

    virtual ~LuaWrapper();

    template<typename ResultType, typename... Args>
    ResultType call(const std::string&, Args&&...);

    template<typename ResultType, typename... Args>
    auto grabAndBoundArgs(const std::string&, Args&&...);

    template<typename ResultType, typename... Args>
    auto grab(const std::string&);

    struct FileNotFoundException : public std::runtime_error
    {
        FileNotFoundException() : std::runtime_error("FileNotFoundException") {}
    };

    struct FunctionNotFoundException : public std::runtime_error
    {
        FunctionNotFoundException() : std::runtime_error("FunctionNotFoundException") {}
    };

protected:
    template<typename T>
    auto pushObjectOnLuaStack(T) noexcept;

    void pushObjectsOnLuaStack() const noexcept {}

    template<typename FirstType>
    void pushObjectsOnLuaStack(FirstType) noexcept;

    template<typename FirstType, typename... Args>
    void pushObjectsOnLuaStack(FirstType, Args...) noexcept;

    template<typename ResultType>
    ResultType getResult() noexcept;

private:
    std::unique_ptr<lua_State, LuaStateDeleter> luaState_;
};

template<typename T>
auto LuaWrapper::pushObjectOnLuaStack(T obj_to_push) noexcept {
    push_on_stack<T>(luaState_.get(), obj_to_push);
}

/*
 * Function pushing arguments on a lua stack.
 * args: Variadic template types.
 *
 * return: Void.
 *
 */
template<typename FirstType>
void LuaWrapper::pushObjectsOnLuaStack(FirstType last_arg) noexcept {
    pushObjectOnLuaStack<FirstType>(last_arg);
}

template<typename FirstType, typename... Args>
void LuaWrapper::pushObjectsOnLuaStack(FirstType first_arg, Args... args) noexcept {
    pushObjectOnLuaStack<FirstType>(first_arg);

    pushObjectsOnLuaStack(args...);
}

/*
 * Provides result of a lua function.
 *
 * result: result of a lua function.
 */
template<typename ResultType>
ResultType LuaWrapper::getResult() noexcept {
    return get_result_from_lua<ResultType>(luaState_.get());
}

/*
 * Calling function lua function  directly.
 *
 * result: Result of a lua function.
 */
template<typename ResultType, typename... Args>
ResultType LuaWrapper::call(const std::string& name, Args &&... args) {
    constexpr size_t resultsNum = 1;

    lua_getglobal(luaState_.get(), name.c_str());
    if (lua_isnil(luaState_.get(), STACK_TOP)) {
        throw FunctionNotFoundException{};
    }

    pushObjectsOnLuaStack(std::forward<Args>(args)...);
    lua_call(luaState_.get(), sizeof...(args), resultsNum);

    return getResult<ResultType>();
}

/*
 * A Function returns a lua function with bound arguments.
 *
 * result: lambda expression.
 */
template<typename ResultType, typename... Args>
auto LuaWrapper::grabAndBoundArgs(const std::string& name, Args &&... args) {
    return [this, name, args...]() -> ResultType {
        return call<ResultType>(name, args...);
    };
}

/*
 * A Function returns a lua function.
 *
 * result: lambda expression.
 */
template<typename ResultType, typename... Args>
auto LuaWrapper::grab(const std::string &name) {
    return [this, name](auto &&... args) -> ResultType {
        return call<ResultType>(name, std::forward<decltype(args)>(args)...);
    };
}
} //namespace lua


#endif //LUAWRAPPER_2019_LUAWRAPPER_H
