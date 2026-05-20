#include "lua.h"
#include "lualib.h"
#include <slc/slc.hpp>

#include "bot/bot.hpp"
#include "bot/updater.hpp"
#include "replay/system.hpp"
#include "script/script.hpp"
#include "game.hpp"

#include <vector>

using namespace geode::prelude;


// game.onUpdate(() -> ()): ()
static int game_onUpdate(lua_State* L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    Script* script = (Script*)lua_touserdata(L, lua_upvalueindex(1));
    script->m_onUpdate.connect(L);
    return 0;
}


// void push_input(lua_State* L, slc::Action input) {
//     lua_newtable(L);
//         lua_pushboolean(L, input.m_holding);
//         lua_setfield(L, -2, "p1");
//         lua_pushboolean(L, input.m_player2);
//         lua_setfield(L, -2, "p2");
// }

void push_input(lua_State* L, PlayerButtonCommand cmd) {
    lua_newtable(L);
        lua_pushboolean(L, cmd.m_isPush);
        lua_setfield(L, -2, "holding");
        lua_pushboolean(L, cmd.m_isPlayer2);
        lua_setfield(L, -2, "isPlayer2");
}
// game.getInputs(tick: number): { {p1: boolean, p2: boolean} }
static int game_getInputs(lua_State* L) {
    auto pl = PlayLayer::get();
    if (!pl) { luaL_error(L, "PlayLayer is not active"); return 0; }
    
    lua_newtable(L);
    for (int i = 0; i < pl->m_queuedButtons.size(); i++) {
        push_input(L, pl->m_queuedButtons[i]);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}


/*
    game global
*/
static int game_index(lua_State* L) {
    Script* script = (Script*)lua_touserdata(L, lua_upvalueindex(1));
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "onUpdate") == 0) {
        lua_pushlightuserdata(L, script);
        lua_pushcclosure(L, game_onUpdate, "onUpdate", 1);
        return 1;
    }
    if (strcmp(key, "getInputs") == 0) {
        lua_pushlightuserdata(L, script);
        lua_pushcclosure(L, game_getInputs, "getInputs", 1);
        return 1;
    }

    Bot *bot = Bot::get();
    if (strcmp(key, "tps") == 0)          lua_pushnumber(L, bot->updater().getTps());
    else if (strcmp(key, "tick") == 0)    lua_pushnumber(L, bot->updater().getFrame());
    else luaL_error(L, "unknown field '%s'", key);

    return 1;
}

void game_register(Script *script) {
    auto L = script->m_state;

    lua_newtable(L); // game

    lua_newtable(L); // game metatable
        lua_pushlightuserdata(L, script);
        lua_pushcclosure(L, game_index, "__index", 1);
        lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);

    lua_setglobal(L, "game");
}