#
# Add unit tests framework
#
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
CPMAddPackage(
  NAME              Unity
  GITHUB_REPOSITORY ThrowTheSwitch/Unity
  VERSION           2.5.1
)
target_compile_definitions(unity PRIVATE UNITY_OUTPUT_COLOR)

#
# Add test target
#
set(UNIT_TESTS "test-strsepf")

add_executable(${UNIT_TESTS})
target_sources(${UNIT_TESTS} PRIVATE test_strsepf.c)

# Modele dependencies -> library under tests + test framework
target_link_libraries(${UNIT_TESTS} PRIVATE ${PROJECT_NAME} unity)

target_compile_options(${UNIT_TESTS}
    PRIVATE
        "-Wall"                     # essential
        "-Wextra"                   # essential
        "-Wpedantic"                # essential
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
target_link_options(${UNIT_TESTS}
    PRIVATE
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
)

#
# Register tests
#
add_test(NAME run-${UNIT_TESTS} COMMAND ${UNIT_TESTS})
