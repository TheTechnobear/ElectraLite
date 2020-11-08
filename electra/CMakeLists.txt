project(electralite)

include_directories(
        "${PROJECT_SOURCE_DIR}/../external/moodycamel"
        "${PROJECT_SOURCE_DIR}/../external/rtmidi"
        include_directories ("${PROJECT_SOURCE_DIR}/../external/nlohmann_json/single_include/nlohmann")
        #        "${PROJECT_SOURCE_DIR}/../../../external/portaudio"
        #        "${PROJECT_SOURCE_DIR}/../../../external/oscpack"
)

set(EL_H
        ElectraDevice.h
        MidiDevice.h
        )

set(EL_SRC
        ElectraDevice.cpp
        MidiDevice.cpp
        )



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

