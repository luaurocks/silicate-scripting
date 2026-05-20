#pragma once

#include "script/script.hpp"

#include <string>
#include <variant>
#include <vector>

struct ScriptButton {
    std::string label;
    int callbackRef = -1;
};

struct ScriptDrag {
    std::string label;
    float value = 0.0f;
    float min = 0.0f, max = 1.0f, speed = 0.01f;
    std::string format = "{:.2f}";
    int callbackRef = -1;
};

struct ScriptCheckbox {
    std::string label;
    bool value = false;
    int callbackRef = -1;
};

using ScriptUIElement = std::variant<ScriptButton, ScriptDrag, ScriptCheckbox>;

struct ScriptUI {
    std::vector<ScriptUIElement> elements;
    void clear() { elements.clear(); }
};