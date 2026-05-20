#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include "lua.h"
#include "lualib.h"
#include "script/ui.hpp"

#include <string>
#include <vector>
#include <filesystem>


struct EventConnection {
    std::vector<int> m_bindRefs; // registry refs to the binded functions

    void dispatch(lua_State* L);
    void connect(lua_State* L);
};

struct Script {
    std::filesystem::path m_scriptPath;
    std::string m_name = "Unnamed script";
    bool m_enabled = true;
    lua_State* m_state = nullptr;
    ScriptUI m_ui = ScriptUI{};

    EventConnection m_onUpdate = EventConnection{};

    std::string chunkName() const {
        return m_scriptPath.filename().string();
    }
    void render();
};

class ScriptManager {
   private:
    void initScript(Script *script);

   public:
    std::vector<std::unique_ptr<Script>> m_scripts;
    void loadScript(std::filesystem::path scriptPath);
    void enableScript(Script& script);
    void disableScript(Script& script);
    void unloadScript(int i);

    void dispatchUpdate();
};

#endif