#include "lua.h"
#include "lualib.h"

#include "script/script.hpp"
#include "player.hpp"

#include <vector>

using namespace geode::prelude;

static PlayerObject* getPlayerByIndex(lua_State* L, int index) {
    auto pl = PlayLayer::get();
    if (!pl) { luaL_error(L, "PlayLayer is not active"); return nullptr; }
    if (index == 1) return pl->m_player1;
    if (index == 2) return pl->m_player2;
    luaL_error(L, "player index must be 1 or 2");
    return nullptr;
}

/*
    Player methods
*/
static int proxy_hold(lua_State* L) {
    int playerIndex = (int)lua_tointeger(L, lua_upvalueindex(1));
    auto pl = PlayLayer::get();
    if (!pl) { luaL_error(L, "PlayLayer is not active"); return 0; }

    PlayerObject* p = getPlayerByIndex(L, playerIndex);
    pl->queueButton(1, !p->m_jumpBuffered, false, 0.0);
    return 0;
}

static int proxy_release(lua_State* L) {
    int playerIndex = (int)lua_tointeger(L, lua_upvalueindex(1));
    auto pl = PlayLayer::get();
    if (!pl) { luaL_error(L, "PlayLayer is not active"); return 0; }

    pl->queueButton(1, false, false, 0.0);
    return 0;
}

/*
    Player metatable
*/
static int proxy_index(lua_State* L) {
    int playerIndex = (int)lua_tointeger(L, lua_upvalueindex(1));
    PlayerObject* p = getPlayerByIndex(L, playerIndex);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "hold") == 0) {
        lua_pushinteger(L, playerIndex);
        lua_pushcclosure(L, proxy_hold, "hold", 1);
        return 1;
    }
    if (strcmp(key, "release") == 0) {
        lua_pushinteger(L, playerIndex);
        lua_pushcclosure(L, proxy_release, "release", 1);
        return 1;
    }

    if (strcmp(key, "x") == 0)             lua_pushnumber(L, p->m_position.x);
    else if (strcmp(key, "y") == 0)        lua_pushnumber(L, p->m_position.y);
    // else if (strcmp(key, "xVel") == 0)     lua_pushnumber(L, p->m_xVelocity);
    else if (strcmp(key, "yVel") == 0)     lua_pushnumber(L, p->m_yVelocity);
    else if (strcmp(key, "rotation") == 0) lua_pushnumber(L, p->getRotation());
    else luaL_error(L, "unknown field '%s'", key);

    return 1;
}
static int proxy_newindex(lua_State* L) {
    int playerIndex = (int)lua_tointeger(L, lua_upvalueindex(1));
    luaL_error(L, "player is read-only");
    return 0;
}

static void pushPlayerProxy(lua_State* L, int playerIndex) {
    lua_newtable(L); // proxy table

    lua_newtable(L); // proxy metatable
        lua_pushinteger(L, playerIndex);
        lua_pushcclosure(L, proxy_index, "__index", 1);
        lua_setfield(L, -2, "__index");

        lua_pushinteger(L, playerIndex);
        lua_pushcclosure(L, proxy_newindex, "__newindex", 1);
        lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, -2);
}

/*
    player global
    Index it like player[1] to get that player object
*/
static int player_index(lua_State* L) {
    int index = (int)luaL_checknumber(L, 2);
    if (index != 1 && index != 2)
        luaL_error(L, "player index must be 1 or 2");
    pushPlayerProxy(L, index);
    return 1;
}

void player_register(Script *script) {
    auto L = script->m_state;
    lua_newtable(L); // player

    lua_newtable(L); // player metatable
        lua_pushcfunction(L, player_index, "__index");
        lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);

    lua_setglobal(L, "player");
}