# basic electratest
set(TEST_SRC "electratest.cpp")
include_directories ("${PROJECT_SOURCE_DIR}/..")
add_executable(electratest ${TEST_SRC})
#target_link_libraries (electratest electralite portaudio)
target_link_libraries (electratest electralite)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(electratest atomic)
endif()
if(APPLE)
target_link_libraries(electratest  "-framework CoreServices -framework CoreFoundation -framework IOKit -framework CoreAudio")
elseif(UNIX) 
target_link_libraries(electratest pthread)
endif(APPLE)

