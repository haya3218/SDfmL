#include "sdfml_lua.hpp"

int sdfml::LuaHandler::create_from_lua_state(lua_State *L, std::string path, bool immediate) {
    _prefer_view = true;
    sol::state_view temp(L);
    _lua_view = temp;
    _cur_result = _lua_view.load_file(path.c_str());
    if (immediate) 
        return 0;

    sol::protected_function_result result = _cur_result();
    return 1 - result.valid();
}

int sdfml::LuaHandler::open_state(std::string path, bool immediate) {
    _prefer_view = false;
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
    if (_prefer_view) _lua_view[var_name] = value; return;
    _lua_state[var_name] = value;
}

template <typename TName>
TName sdfml::LuaHandler::get_variable(std::string var_name) {
    if (_prefer_view) return _lua_state.get<TName>(var_name);
    return _lua_state.get<TName>(var_name);
}

template <typename... Args>
void sdfml::LuaHandler::add_libraries(Args&&... libraries) {
    if (_prefer_view) _lua_view.open_libraries(libraries...); return;
    _lua_state.open_libraries(libraries...);
}