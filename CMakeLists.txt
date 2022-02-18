# Make sur cmake is 3.10
cmake_minimum_required(VERSION 3.10)

# proj name
project(Skateboard)

# include directories
set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SDL2/")
set(OUT_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/")
set(OUT2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/")
set(OUT3_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/toml/")
set(OUT4_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/sdfml/")
set(OUT5_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SDL_gpu/")

option(PREFER_MODPLUG "Use libmodplug api instead of libopenmpt for module playback" ON)

if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	set(MAIN_LIBRARIES_E "${CMAKE_CURRENT_LIST_DIR}/lib/x64")
else ()
	set(MAIN_LIBRARIES_E "${CMAKE_CURRENT_LIST_DIR}/lib/x86")
endif ()

# Support both 32 and 64 bit builds
# Someone make a pull request to support MingW
# Way to lazy to do that right now
set(MAIN_LIBRARIES "${MAIN_LIBRARIES_E}/SDL2_gpu.lib;${MAIN_LIBRARIES_E}/libmodplug.lib;${MAIN_LIBRARIES_E}/soloud_static.lib;${MAIN_LIBRARIES_E}/SDL2.lib;${MAIN_LIBRARIES_E}/SDL2_ttf.lib")

# strip it all
string(STRIP "${MAIN_LIBRARIES}" MAIN_LIBRARIES)

# Include the damn directories
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OUT_INCLUDE_DIRS})
include_directories(${OUT2_INCLUDE_DIRS})
include_directories(${OUT3_INCLUDE_DIRS})
include_directories(${OUT4_INCLUDE_DIRS})
include_directories(${OUT5_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

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
)

# set c++ std to 17
set(CMAKE_CXX_STANDARD 17)

# edit executable properties part 1
add_executable(Skateboard WIN32 ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/resources/res.rc")

# COMPILE DEFINES
target_compile_definitions(Skateboard PRIVATE PREFER_MODPLUG)

# LINK EVERYTHING
target_link_libraries(Skateboard ${MAIN_LIBRARIES})

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
file(COPY "${MAIN_LIBRARIES_E}/" DESTINATION ${BUILD_DIRECTORY})

# remove lib files
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/*.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2_ttf.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2main.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2test.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2_gpu.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/soloud_static.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/libmodplug.lib")

if (PREFER_MODPLUG)
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/libopenmpt.dll")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/openmpt-mpg123.dll")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/openmpt-ogg.dll")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/openmpt-vorbis.dll")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/openmpt-zlib.dll")
endif()

# :)
