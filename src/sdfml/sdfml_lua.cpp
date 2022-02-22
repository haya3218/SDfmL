#include "sdfml_lua.hpp"

int sdfml::LuaHandler::open_state(std::string path, bool immediate) {
    _cur_result = _lua_state.load_file(path.c_str());
    if (immediate) 
        return 0;

    sol::protected_function_result result = _cur_result();
    return 1 - result.valid();
}

int sdfml::LuaHandler::run_state() {
    sol::protected_function_result result = _cur_result();
    return 1 - result.valid();
}

template <typename TName>
void sdfml::LuaHandler::set_variable(std::string var_name, TName value) {
    _lua_state[var_name] = value;
}

template <typename TName>
TName sdfml::LuaHandler::get_variable(std::string var_name) {
    return _lua_state.get<TName>(var_name);
}

template <typename... Args>
void sdfml::LuaHandler::add_libraries(Args&&... libraries) {
    _lua_state.open_libraries(libraries...);
}