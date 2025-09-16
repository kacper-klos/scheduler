cmake_minimum_required(VERSION 3.16)
project(scheduler_gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_executable(scheduler_gui src/gui/main_widget.cpp src/gui/calendar.cpp
                             src/gui/event_creator.cpp)

target_link_libraries(scheduler_gui PRIVATE Qt6::Widgets)
target_include_directories(scheduler_gui PRIVATE include)
