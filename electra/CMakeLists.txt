project(electralite)


include_directories(
        "${PROJECT_SOURCE_DIR}/../external/moodycamel"
        "${PROJECT_SOURCE_DIR}/../external/rtmidi"
        include_directories ("${PROJECT_SOURCE_DIR}/../external/libremidi/include")
        include_directories ("${PROJECT_SOURCE_DIR}/../external/nlohmann_json/single_include/nlohmann")
)


set(EL_H
        ElectraDevice.h
        MidiDevice.h
        RtMidiDevice.h
        )

set(EL_SRC
        ElectraDevice.cpp
        MidiDevice.cpp
        RtMidiDevice.cpp
        )


if(USE_LIBRE_MIDI)
    message("use libre midi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBREMIDI_HEADER_ONLY=1 -DUSE_LIBRE_MIDI=1")
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBREMIDI_COREAUDIO=1")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBREMIDI_ALSA=1")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBREMIDI_WMIDI=1")
    endif()
    set(EL_H ${EL_H} LibreMidiDevice.h)
    set(EL_SRC ${EL_SRC} LibreMidiDevice.cpp)
endif()







add_library(electralite SHARED ${EL_SRC} ${EL_H})

# target_link_libraries(electralite oscpack portaudio)
target_link_libraries(electralite rtmidi)
target_link_libraries(electralite nlohmann_json::nlohmann_json)
# target_link_libraries(electralite PRIVATE nlohmann_json::nlohmann_json)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(electralite atomic)
endif ()

target_include_directories(electralite PUBLIC
        .
        )

add_subdirectory(tests)

