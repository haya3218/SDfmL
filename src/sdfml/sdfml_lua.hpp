#ifndef LUA_SDFML_IMPLEMENTATION
#define LUA_SDFML_IMPLEMENTATION

#include <string>
#include <functional>
#include "../lua/sol.hpp"

namespace sdfml {
    typedef std::function<void> LFunc;

    class LuaHandler {
        protected:
            sol::state _lua_state;
            sol::state_view _lua_view;
            sol::load_result _cur_result;
        public:
            int open_state(std::string path, bool immediate = false);

            int run_state();

            template <typename TName = int>
            void set_variable(std::string var_name, TName value);

            template <typename TName = int>
            TName get_variable(std::string var_name);

            template <typename... Args>
            void add_libraries(Args&&... libraries);
    };
}

#endif