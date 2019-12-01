#---------------------------------------------------------------------------------------
# C projets using the WSL
# No dependencies other than CMAKE, make and gcc.
# 
# G.Berthiaume - 2019
#---------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)


###############################################################################
## language ###################################################################
###############################################################################
project(strsepf_project VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# cflags for the compiler
list(
    APPEND 
    cflags
    "-Wall"                     # essential
    "-Wextra"                   # essential
    "-Wpedantic"                # essential
    "-pedantic-errors"          # essential
    "-Wpointer-arith"           # warn if anything depends upon the size of a function or of void.
    "-Wcast-align"              # warn whenever a pointer is cast such that the required alignment of the target is increased. 
    "-Wfloat-equal"             # because usually testing floating-point numbers for equality is bad.
    "-Wshadow"                  # warn whenever a local variable shadows another local variable, parameter or global variable 
    "-Wcast-align"              # warn whenever a pointer is cast such that the required alignment of the target is increased. 
    "-Wswitch-default"          # warn whenever a switch statement does not have a default case.
    "-Wwrite-strings"           # give string constants the type const char[length] so that copying the address of one into a non-const char * pointer will get a warning.
    "-Wswitch-enum"             # warn whenever a switch statement has an index of enumerated type and lacks a case for one or more of the named codes of that enumeration*.
    "-Wunreachable-code"        # warn if the compiler detects that code will never be executed.
    "-O2"                       # Compile with normal optimisation
    "-g"
    "-ggdb"
    "-fsanitize=address"        # address sanitizer flags -> Your new best friends !
    "-fno-omit-frame-pointer"   # address sanitizer flags -> Your new best friends !
)

message(STATUS "[INFO] Compiler flags:")
foreach(cflag IN LISTS cflags)
    message("   · ${cflag}")
endforeach() 
message("")



###############################################################################
## file globbing ##############################################################
###############################################################################

# These instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables 
# `sources` and `data`.
file(GLOB_RECURSE sources        src/*.c src/*.h)
file(GLOB_RECURSE sources_test   test/*.c)

list(LENGTH sources sources_count)

message(STATUS "[INFO] Found ${sources_count} source files:")
foreach(f IN LISTS sources)
    get_filename_component(relf ${f} NAME)
    message("   · ${relf}")
endforeach() 
message("")


###############################################################################
## Build libraries ############################################################
###############################################################################

# generate a library from sources
add_library(
    lib_target
    STATIC
    ${sources}
)
target_compile_options(
    lib_target
    PRIVATE
    ${cflags}
)
# We need to add the address sanitizer flags to the linker flag too.
# In cmake 3.13, we could use `target_link_options`.
# But the WSL currently come with 3.10 installed by default.
set_target_properties(
    lib_target
    PROPERTIES
    LINK_FLAGS "-fsanitize=address -fno-omit-frame-pointer"
    LINKER_LANGUAGE C # This helps if we only have a single header libs.
)
target_include_directories(
    lib_target
    PUBLIC
    src
)

###############################################################################
## Build external test framework ##############################################
###############################################################################

add_library(
    lib_unity
    STATIC
    ../external/Unity/src/unity.c
)
target_compile_options(
    lib_unity
    PRIVATE
    -DUNITY_OUTPUT_COLOR # Force Unity color
)
target_include_directories(
    lib_unity
    PUBLIC
    external/Unity/src/ # So we can just use "unity.h"
)


###############################################################################
## Build tests ################################################################
###############################################################################

# enable testing functionality
enable_testing()

# Build all tests
foreach(test_file IN LISTS sources_test)
    get_filename_component(test_file_name ${test_file} NAME_WE)
    message(STATUS "Creating `${test_file_name}` test bench.")

    set(STATICLIB_NAME "run_${test_file_name}")
    add_executable(${STATICLIB_NAME} ${test_file})

    target_link_libraries(${STATICLIB_NAME} lib_target)  # Library under tests
    target_link_libraries(${STATICLIB_NAME} lib_unity)
    target_compile_options(
        ${STATICLIB_NAME}
        PRIVATE
        ${cflags}
    )
    set_target_properties( 
        ${STATICLIB_NAME}
        PROPERTIES
        LINK_FLAGS "-fsanitize=address -fno-omit-frame-pointer"
        LINKER_LANGUAGE C   # This helps if we only have a single header libs.
    )

    # define tests
    add_test(
        NAME ${STATICLIB_NAME}
        COMMAND ./${STATICLIB_NAME}
    )
endforeach() 
