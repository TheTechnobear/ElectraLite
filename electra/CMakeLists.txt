project(electralite)

include_directories(
        "${PROJECT_SOURCE_DIR}/../external/moodycamel"
        "${PROJECT_SOURCE_DIR}/../external/cJSON"
        "${PROJECT_SOURCE_DIR}/../external/rtmidi"
        #        "${PROJECT_SOURCE_DIR}/../../../external/portaudio"
        #        "${PROJECT_SOURCE_DIR}/../../../external/oscpack"
)

set(EL_H
        ElectraDevice.h
        )

set(EL_SRC
        ElectraDevice.cpp
        )



add_library(electralite SHARED ${EL_SRC} ${EL_H})

# target_link_libraries(electralite oscpack portaudio cjson)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(electralite atomic)
endif ()

target_include_directories(electralite PUBLIC
        .
        )

add_subdirectory(tests)


