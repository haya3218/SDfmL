# Make sur cmake is 3.8
cmake_minimum_required(VERSION 3.8)

# proj name
project(Skateboard)

# include directories
set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SDL2/")
set(OUT_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/")
set(OUT2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/")
set(OUT3_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/toml/")

# Support both 32 and 64 bit builds
# Someone make a pull request to support MingW
# Way to lazy to do that right now
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  set(MAIN_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/soloud_static.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_ttf.lib")
  set(MAIN_LIBRARIES_E "${CMAKE_CURRENT_LIST_DIR}/lib/x64/")
else ()
  set(MAIN_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/soloud_static.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_ttf.lib")
  set(MAIN_LIBRARIES_E "${CMAKE_CURRENT_LIST_DIR}/lib/x86/")
endif ()


# strip it all
string(STRIP "${MAIN_LIBRARIES}" MAIN_LIBRARIES)

# Include the damn directories
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OUT_INCLUDE_DIRS})
include_directories(${OUT2_INCLUDE_DIRS})
include_directories(${OUT3_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# add EVERY FUCKING source file to SOURCES
FILE(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" 
"${CMAKE_CURRENT_LIST_DIR}/src/*.hpp"  
"${CMAKE_CURRENT_LIST_DIR}/src/*.c"  
"${CMAKE_CURRENT_LIST_DIR}/src/*.h"  
"${CMAKE_CURRENT_LIST_DIR}/src/SDL2/*.c"  
"${CMAKE_CURRENT_LIST_DIR}/src/SDL2/*.h"  
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_audiosource.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_bassboostfilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_biquadresonantfilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_bus.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_c.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_dcremovalfilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_echofilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_error.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_fader.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_fft.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_fftfilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_file_hack_off.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_file_hack_on.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_file.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_filter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_flangerfilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_freeverbfilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_internal.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_lofifilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_misc.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_monotone.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_noise.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_openmpt.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_queue.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_robotizefilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_sfxr.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_speech.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_tedsid.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_thread.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_vic.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_vizsn.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_wav.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_waveshaperfilter.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud_wavstream.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/soloud.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/tml.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/tsf.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/soloud_midi.h"
"${CMAKE_CURRENT_LIST_DIR}/src/SoLoud/MIDI/*.cpp"
"${CMAKE_CURRENT_LIST_DIR}/src/toml/*.hpp"
)

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
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2_ttf.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2main.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/SDL2test.lib")
file(REMOVE_RECURSE "${BUILD_DIRECTORY}/soloud_static.lib")

# :)