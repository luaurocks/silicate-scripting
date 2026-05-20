#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "script.hpp"
#include "script/api/game.hpp"
#include "script/api/player.hpp"
#include "script/api/ui.hpp"
#include "bot/bot.hpp"

#include <string>
#include <optional>
#include <vector>
#include <filesystem>
#include <sstream>

using namespace geode::prelude;


/*
    Scripting API
*/
// log(...any)
static int l_log(lua_State* L) {
    size_t len;
    const char* str = luaL_tolstring(L, 1, &len);
    geode::log::info("{}", str);
    lua_pop(L, 1);
    return 0;
}

void EventConnection::dispatch(lua_State* L) {
    for (int ref : m_bindRefs) {
        lua_getref(L, ref);
        if (lua_pcall(L, 0, 0, 0) != 0) {
            geode::log::error("onUpdate error: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
}
void EventConnection::connect(lua_State* L) {
    lua_pushvalue(L, 1);
    int ref = lua_ref(L, -1);
    lua_pop(L, 1);
    m_bindRefs.push_back(ref);
}

void Script::render() {
    ui_render(this);
}


/*
    Adding scripts
*/
void ScriptManager::loadScript(std::filesystem::path scriptPath) {
    // read the file
    std::ifstream file(scriptPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // create the script and lua state
    auto script = std::make_unique<Script>();
    script->m_scriptPath = scriptPath;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L); // maybe remove that?
    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(content.c_str(), content.length(), nullptr, &bytecodeSize);

    if (luau_load(
        L,
        scriptPath.filename().string().c_str(),
        bytecode,
        bytecodeSize,
        0
    ) != 0) {
        geode::log::error("Failed to load script: {}", lua_tostring(L, -1));
        return;
    }

    script->m_state = L;

    this->m_scripts.push_back(std::move(script));
    
    // bye
    auto& s = *m_scripts.back();
    this->initScript(m_scripts.back().get());
    geode::log::info("Added script: {}", s.chunkName());
    this->enableScript(s);
    free(bytecode);
}

void ScriptManager::initScript(Script *script) {
    lua_State* L = script->m_state;
    lua_pushcfunction(L, l_log, "log");
    lua_setglobal(L, "log");

    game_register(script);
    player_register(script);
    ui_register(script);
}

void ScriptManager::unloadScript(int i) {
    auto &script = *this->m_scripts[i];
    geode::log::debug("Unloading script: {}", script.chunkName());
    this->disableScript(script);
    this->m_scripts.erase(this->m_scripts.begin() + i);
}


/*
    Managing scripts
*/
void ScriptManager::enableScript(Script& script) {
    if (script.m_state == nullptr) {
        geode::log::error("Failed to enable script {}, luau state is null", script.chunkName());
        return;
    }

    script.m_enabled = true; // if it hasn't been set yet
    lua_pcall(script.m_state, 0, 0, 0);
    geode::log::debug("Enabled script: {}", script.chunkName());
}

void ScriptManager::disableScript(Script& script) {
    script.m_enabled = false;
    // if (script.m_state != nullptr) {
    //     lua_close(script.m_state);
    //     script.m_state = nullptr;
    // }
}


/*
    Updater
*/
void ScriptManager::dispatchUpdate() {
    for (auto &script : this->m_scripts) {
        if (script->m_enabled == false) continue;
        if (script->m_state == nullptr) continue; // shouldn't happen (pls)
        script->m_onUpdate.dispatch(script->m_state);
    }
}