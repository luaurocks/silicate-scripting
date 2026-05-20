#include "script/script.hpp"
#include "script/ui.hpp"
#include "script/api/ui.hpp"
#include <tabby.hpp>

#include <string>
#include <variant>
#include <vector>

// ui.button(label: string, callback: (val: string)->()): ()
static int ui_button(lua_State* L) {
    Script* script = (Script*)lua_touserdata(L, lua_upvalueindex(1));
    const char* label = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    lua_pushvalue(L, 2);
    int ref = lua_ref(L, -1);
    lua_pop(L, 1);

    ScriptButton btn;
    btn.label = label;
    btn.callbackRef = ref;
    script->m_ui.elements.push_back(btn);
    return 0;
}

// ui.drag(label: string, value: number, min: number, max: number, speed: number, callback: (val: number)->()): ()
static int ui_drag(lua_State* L) {
    Script* script = (Script*)lua_touserdata(L, lua_upvalueindex(1));
    const char* label = luaL_checkstring(L, 1);
    float value       = (float)luaL_checknumber(L, 2);
    float min         = (float)luaL_checknumber(L, 3);
    float max         = (float)luaL_checknumber(L, 4);
    float speed       = (float)luaL_checknumber(L, 5);
    luaL_checktype(L, 6, LUA_TFUNCTION);

    lua_pushvalue(L, 6);
    int ref = lua_ref(L, -1);
    lua_pop(L, 1);

    ScriptDrag drag;
    drag.label = label;
    drag.value = value;
    drag.min = min;
    drag.max = max;
    drag.speed = speed;
    drag.callbackRef = ref;
    script->m_ui.elements.push_back(drag);
    return 0;
}

// ui.checkbox(label: string, initialValue: boolean, callback: (val: boolean)->()): ()
static int ui_checkbox(lua_State* L) {
    Script* script = (Script*)lua_touserdata(L, lua_upvalueindex(1));
    const char* label = luaL_checkstring(L, 1);
    bool value        = lua_toboolean(L, 2);
    luaL_checktype(L, 3, LUA_TFUNCTION);

    lua_pushvalue(L, 3);
    int ref = lua_ref(L, -1);
    lua_pop(L, 1);

    ScriptCheckbox cb;
    cb.label = label;
    cb.value = value;
    cb.callbackRef = ref;
    script->m_ui.elements.push_back(cb);
    return 0;
}


// lua helper funcs
static void fireCallback(lua_State* L, int vals) {
    if (lua_pcall(L, vals, 0, 0) != 0) {
        geode::log::error("ui callback error: {}", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

/*
	hi future extreme820 or whoever is reading this
	drag and cb work because tabby expects a reference to a value
	we have our own class for drag and checkbox, but we pass it
	tabby::checkbox(cb->label, cb->value);
	cb->value here is a reference and checkbox() can change it
*/
void ui_render(Script* script) {
    auto L = script->m_state;
    for (auto& el : script->m_ui.elements) {
        if (auto* btn = std::get_if<ScriptButton>(&el)) {
            if (!tabby::button(btn->label).pressed) continue;
		    lua_getref(L, btn->callbackRef);
            fireCallback(L, 0);
        }

        else if (auto* drag = std::get_if<ScriptDrag>(&el)) {
            auto result = tabby::drag(drag->label, drag->value, drag->min, drag->max, drag->speed, drag->format);
            if (!result.changed) return;
            lua_getref(L, drag->callbackRef);
            lua_pushnumber(L, drag->value);
            fireCallback(L, 1);
        }

        else if (auto* cb = std::get_if<ScriptCheckbox>(&el)) {
            auto result = tabby::checkbox(cb->label, cb->value);
            if (!result.changed) continue;
            lua_getref(L, drag->callbackRef);
            lua_pushnumber(L, cb->value);
            fireCallback(L, 1);
        }
    }
}

/*
	ui global
*/
static int ui_index(lua_State* L) {
    Script* script = (Script*)lua_touserdata(L, lua_upvalueindex(1));
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "button") == 0) {
        lua_pushlightuserdata(L, script);
        lua_pushcclosure(L, ui_button, "button", 1);
    } else if (strcmp(key, "drag") == 0) {
        lua_pushlightuserdata(L, script);
        lua_pushcclosure(L, ui_drag, "drag", 1);
    } else if (strcmp(key, "checkbox") == 0) {
        lua_pushlightuserdata(L, script);
        lua_pushcclosure(L, ui_checkbox, "checkbox", 1);
    } else {
        luaL_error(L, "unknown ui field '%s'", key);
    }
    return 1;
}

void ui_register(Script* script) {
    auto L = script->m_state;
    lua_newtable(L); // ui

    lua_newtable(L); // metatable
        lua_pushlightuserdata(L, script);
        lua_pushcclosure(L, ui_index, "__index", 1);
        lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);

    lua_setglobal(L, "ui");
}