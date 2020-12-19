
include_directories ("${PROJECT_SOURCE_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/../external/nlohmann_json/single_include/nlohmann")

# e1_test_1
set(e1_test_1_src "e1_test_1.cpp")
add_executable(e1_test_1 ${e1_test_1_src})
#target_link_libraries (e1_test_1 electralite portaudio)
target_link_libraries (e1_test_1 rtmidi)
target_link_libraries (e1_test_1 nlohmann_json::nlohmann_json)
target_link_libraries (e1_test_1 electralite)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(e1_test_1 atomic)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(e1_test_1  "-framework CoreServices -framework CoreFoundation -framework IOKit -framework CoreAudio")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(e1_test_1 pthread)
endif()




# e1_test_2
set(e1_test_2_src "e1_test_2.cpp")
add_executable(e1_test_2 ${e1_test_2_src})
#target_link_libraries (e1_test_2 electralite portaudio)
target_link_libraries (e1_test_2 rtmidi)
target_link_libraries (e1_test_2 nlohmann_json::nlohmann_json)
target_link_libraries (e1_test_2 electralite)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(e1_test_2 atomic)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(e1_test_2  "-framework CoreServices -framework CoreFoundation -framework IOKit -framework CoreAudio")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(e1_test_2 pthread)
endif()
