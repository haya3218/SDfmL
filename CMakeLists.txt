# Make sur cmake is 3.10
cmake_minimum_required(VERSION 3.10)

# proj name
project(Skateboard)

# include directories
set(INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SDL2/" 
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/"
"${CMAKE_CURRENT_LIST_DIR}/src/toml/"
"${CMAKE_CURRENT_LIST_DIR}/src/sdfml/"
"${CMAKE_CURRENT_LIST_DIR}/src/SDL_gpu/"
"${CMAKE_CURRENT_LIST_DIR}/src/lua/lua/"
"${CMAKE_CURRENT_LIST_DIR}/src/lua/"
)

option(PREFER_MODPLUG "Use libmodplug api instead of libopenmpt for module playback" ON)

# Lib paths
set(suffix "${CMAKE_CURRENT_LIST_DIR}/lib")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	set(LIB_PATH "${suffix}/x64")
else ()
	set(LIB_PATH "${suffix}/x86")
endif ()

# Lib files
# MingW supports .lib files now I believe??
set(MAIN_LIBRARIES "luajit.lib"
"lua51.lib"
"SDL2_gpu.lib"
"libmodplug.lib"
"soloud_static.lib"
"SDL2.lib"
"SDL2_ttf.lib")

# strip it all
string(STRIP "${MAIN_LIBRARIES}" MAIN_LIBRARIES)

# Include the damn directories
include_directories(${INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# library dirs
link_directories(${LIB_PATH})

# add EVERY FUCKING source file to SOURCES
FILE(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" 
"${CMAKE_CURRENT_LIST_DIR}/src/*.hpp"  
"${CMAKE_CURRENT_LIST_DIR}/src/*.h"  
"${CMAKE_CURRENT_LIST_DIR}/src/SDL2/*.c"  
"${CMAKE_CURRENT_LIST_DIR}/src/SDL2/*.h"  
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_modplug.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/*.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/tml.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/tsf.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/soloud_midi.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/*.cpp"
"${CMAKE_CURRENT_LIST_DIR}/src/toml/*.hpp"
"${CMAKE_CURRENT_LIST_DIR}/src/sdfml/*.cpp" 
"${CMAKE_CURRENT_LIST_DIR}/src/sdfml/*.hpp"  
"${CMAKE_CURRENT_LIST_DIR}/src/SDL_gpu/*.h"  
"${CMAKE_CURRENT_LIST_DIR}/src/lua/*.hpp"  
"${CMAKE_CURRENT_LIST_DIR}/src/lua/lua/*.h"  
)

FILE(GLOB DLLS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")

# set c++ std to 17
set(CMAKE_CXX_STANDARD 17)

# edit executable properties part 1
add_executable(Skateboard WIN32 ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/resources/res.rc")

# COMPILE DEFINES
target_compile_definitions(Skateboard PRIVATE PREFER_MODPLUG)

if ( MSVC )
	target_compile_options(Skateboard PRIVATE /W1)
endif()

# MingW : for a lib file to not require lib before the name
if (MINGW)
	target_link_libraries(Skateboard ":${MAIN_LIBRARIES}")
else()
	target_link_libraries(Skateboard ${MAIN_LIBRARIES})
endif()

# set build dir
set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# actually set build dir
set_target_properties(Skateboard PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIRECTORY}
)

# copy dll files
file(COPY "${LIB_PATH}/" DESTINATION ${BUILD_DIRECTORY})

# remove lib files
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/*.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2_ttf.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2main.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2test.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2_gpu.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/soloud_static.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/libmodplug.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/lua51.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/luajit.lib")

if (PREFER_MODPLUG)
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/libopenmpt.dll")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/openmpt-mpg123.dll")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/openmpt-ogg.dll")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/openmpt-vorbis.dll")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/openmpt-zlib.dll")
endif()

# :)
