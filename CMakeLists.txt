# Make sur cmake is 3.8
cmake_minimum_required(VERSION 3.8)

# proj name
project(Skateboard)

# SDL2 include directories
set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SDL2/")
set(OUT_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/BASS/")
set(BASS_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/BASS/BASS/")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  set(MAIN_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_image.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_ttf.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/bass.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/bass_fx.lib")
  set(MAIN_LIBRARIES_E "${CMAKE_CURRENT_LIST_DIR}/lib/x64/")
else ()
  set(MAIN_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_image.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_ttf.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/bass.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/bass_fx.lib")
  set(MAIN_LIBRARIES_E "${CMAKE_CURRENT_LIST_DIR}/lib/x86/")
endif ()

# strip it all
string(STRIP "${MAIN_LIBRARIES}" MAIN_LIBRARIES)

# Include the damn directories
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OUT_INCLUDE_DIRS})
include_directories(${BASS_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# add EVERY FUCKING source file to SOURCES
FILE(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" 
"${CMAKE_CURRENT_LIST_DIR}/src/*.hpp"  
"${CMAKE_CURRENT_LIST_DIR}/src/BASS/audio_out.h" 
"${CMAKE_CURRENT_LIST_DIR}/src/BASS/audio_fx.h" 
"${CMAKE_CURRENT_LIST_DIR}/src/BASS/audio_out.cc" 
"${CMAKE_CURRENT_LIST_DIR}/src/BASS/audio_fx.cc")

# set c++ std to 17
set(CMAKE_CXX_STANDARD 17)

# edit executable properties part 1
add_executable(Skateboard ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/resources/res.rc")

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
file(COPY ${MAIN_LIBRARIES_E} DESTINATION ${BUILD_DIRECTORY})

# remove lib files
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/*.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2_image.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2_ttf.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2main.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2test.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/bass.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/bass_fx.lib")

# :)