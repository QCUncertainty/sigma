set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_BUILD_TYPE Debug)
set(BUILD_TESTING ON)
set(BUILD_DOCS ON)

# Use whichever version of clang is the default for the OS
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
