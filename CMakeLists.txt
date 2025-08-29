cmake_minimum_required(VERSION 3.16)
project(scheduler_gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_executable(scheduler_gui src/gui/main_widget.cpp)

target_link_libraries(scheduler_gui PRIVATE Qt6::Widgets)
target_include_directories(scheduler_gui PRIVATE include)
