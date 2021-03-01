cmake_minimum_required (VERSION 3.1 FATAL_ERROR)

project(qtesplamp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
set(CMAKE_WIN32_EXECUTABLE ON)
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

add_subdirectory(ThirdParty/colordialog)

file(GLOB SRCS "src/*.cpp" "src/*.ui" "include/*.hpp" "src/res/*")
add_executable(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} colordialog Qt5::Widgets Qt5::Network)

