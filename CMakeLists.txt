cmake_minimum_required(VERSION 3.21)
project(Oving_2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/Users/Thadsha/Nettverksprogrammering/Programmering/Oving 2")

find_package(Qt5 COMPONENTS
        Core
        REQUIRED)

add_executable(Oving_2 main.cpp)
target_link_libraries(Oving_2
        Qt5::Core
        )

