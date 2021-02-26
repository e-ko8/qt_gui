#
# Copyright (C) 2013-2020 Mattia Basaglia
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
cmake_minimum_required (VERSION 3.1 FATAL_ERROR)
if ( ${CMAKE_VERSION} VERSION_GREATER "3.12.0")
    cmake_policy(SET CMP0077 NEW)
endif()

project(colordialog)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
set(CMAKE_WIN32_EXECUTABLE ON)
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets)

file(GLOB SRCS "src/*.cpp" "src/*.ui" "include/*.hpp" "src/res/*")
add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

add_executable(${PROJECT_NAME}_test "src/test/main.cpp" "src/test/main.h")
target_include_directories(${PROJECT_NAME}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_test Qt5::Widgets ${PROJECT_NAME})

