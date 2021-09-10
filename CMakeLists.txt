cmake_minimum_required(VERSION 2.8.12)

project(electralite)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

set(PROJECT_NAME electralite)
set(EL_VERSION_MAJOR 0)
set(EL_VERSION_MINOR 1)
set(EL_VERSION_UPDATE 0)
set(EL_VERSION "${EL_VERSION_MAJOR}.${EL_VERSION_MINOR}.${EL_VERSION_UPDATE}")


if(CMAKE_SYSTEM_NAME STREQUAL "Darwin") 
    set(EXECUTABLE_NAME ElectraLite)
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64")
#        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(EXECUTABLE_NAME electralite)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("windows not supported at this time")
endif()

set(CMAKE_CXX_STANDARD 11)

#set(USE_LIBRE_MIDI on)
set(USE_LIBRE_MIDI off)

if(USE_LIBRE_MIDI)
    set(CMAKE_CXX_STANDARD 17)
endif()


set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -funroll-loops -fomit-frame-pointer")

if (SALT) 
  set(BELA on)
endif (SALT)

if (PEPPER) 
  set(BELA on)
endif (PEPPER)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^armv6l")
    set(ARM6 on)
endif() 

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
if (NOT DEFINED CPU_OPTIMIZATION_FLAGS)
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin") 
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
            if (BELA)
                message(STATUS "BELA optimized")
                # /usr/xenomai/bin/xeno-config --skin=cobalt --no-mode-check --cflags
                if(SALT) 
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSALT")
                endif(SALT) 
                if(PEPPER) 
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPEPPER")
                endif(PEPPER) 

                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/xenomai/include/cobalt -I/usr/xenomai/include -march=armv7-a -D_GNU_SOURCE -D_REENTRANT -D__COBALT__")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ftree-vectorize")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBELA")
                    #  /usr/xenomai/bin/xeno-config --skin=cobalt --no-auto-init --no-mode-check --ldflags
                if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
                else()
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsingle-precision-constant --fast-math")
                endif()

                set(BELA_LINK_FLAGS "-v ${BELA_LINK_FLAGS} -Wl,@${PROJECT_SOURCE_DIR}/xenomai.wrappers -Wl,--no-as-needed -L/usr/xenomai/lib -lcobalt -lmodechk -lpthread -lrt")
                set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BELA_LINK_FLAGS}")
                set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${BELA_LINK_FLAGS}")
                set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${BELA_LINK_FLAGS}")
                #set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${BELA_LINK_FLAGS} ")
            elseif (ARM6)
                message(STATUS "arm6 build")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv6+fp -mfpu=vfp -mfloat-abi=hard")
            else ()
                message(STATUS "arm7 build")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfpu=neon -mfloat-abi=hard")
            endif ()        
        else ()
                # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall")
        endif ()
    endif ()
else()
    message(STATUS "Using provided optimization flags '${CPU_OPTIMIZATION_FLAGS}'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPU_OPTIMIZATION_FLAGS}")
endif ()

if (NOT DISABLE_RPATH_OVERRIDE)
  ## setup rpath for linux and macos
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin") 
    set(CMAKE_INSTALL_NAME_DIR "@rpath")
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib;@loader_path")
    set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR TRUE)
  else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/:$ORIGIN/../lib")
  endif()
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/bin)

############

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(external/nlohmann_json)
add_subdirectory(external/rtmidi rtmidi)
add_subdirectory(external/moodycamel)

#add_subdirectory(external/oscpack oscpack)
#add_subdirectory(external/portaudio portaudio)


if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message("system : macos ")
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("system : linux ")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("system : windows ")
    message("windows support limited")
endif()

add_subdirectory(electra)

