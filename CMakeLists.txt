#-----------------------------------------------------
# Build strsepf
# Usage:
#       mkdir build && cd build
#       cmake ..
#
#       mkdir buildtests && cd buildtests
#       cmake .. -DBUILD_TESTING=ON
#
#
# G.Berthiaume - 2019
#-----------------------------------------------------
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project("strsepf" 
        DESCRIPTION  "A header only C11 library that provides string parsing, combining the memory safety of strsep and the convenience of a sscanf-like interface."
        HOMEPAGE_URL "https://github.com/g-berthiaume/strsepf"
        VERSION      1.0.0
        LANGUAGES    C)

#
# Build options
#
option(BUILD_TESTING "Build unit tests" OFF)

#
# Languages
#
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)


#
# Build project
#
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE src/)


#
# Unit testing
#
if(BUILD_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(test/)
endif()
