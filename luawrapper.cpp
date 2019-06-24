//
// Created by MrLukashem on 22.03.2019.
//

#include "LuaWrapper.h"

namespace lua {
LuaWrapper::LuaWrapper(const std::string& lua_file_source) {
    luaState_ = std::unique_ptr<lua_State, LuaStateDeleter>(luaL_newstate(),
            [](auto luaState) {
                lua_close(luaState);
            });
    luaL_openlibs(luaState_.get());

    const auto isOpened = luaL_dofile(luaState_.get(), lua_file_source.c_str());
    if (isOpened != LUA_OK) {
        throw FileNotFoundException{};
    }
}

LuaWrapper::~LuaWrapper() = default;

} // namespace lua