cmake_minimum_required(VERSION 3.6)

project(SimpleOscilloscope VERSION 0.2 LANGUAGES CXX)

# warn user if system is not UNIX
if (NOT UNIX)
  message (FATAL_ERROR "| SimpleOscilloscope > This is an unsupported system.")
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

add_executable(simpleOscilloscope src/main.cpp src/device/deviceBase.cpp src/device/deviceUART.cpp src/device/deviceFake.cpp src/logs/logs.cpp src/timer.cpp)
target_link_libraries(simpleOscilloscope ${OpenCV_LIBS} Threads::Threads)