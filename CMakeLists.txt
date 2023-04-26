cmake_minimum_required(VERSION 3.6)
project(Phoenix-Pro-Example)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Prefer -lpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Required packages
find_package(phoenix-pro REQUIRED)
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)

# NOTE: The units library requires either <fmt/format.h> or <iostream>, use iostream
add_definitions(-DUNIT_LIB_DISABLE_FMT -DUNIT_LIB_ENABLE_IOSTREAM)

# Add all CPP files to the executable
# Note: Users using GameController should swap out Joystick.cpp with GameController.cpp
add_executable(${PROJECT_NAME} main.cpp RobotBase.cpp Joystick.cpp)

# Specify libraries to link against
target_link_libraries(${PROJECT_NAME} phoenix-pro)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
