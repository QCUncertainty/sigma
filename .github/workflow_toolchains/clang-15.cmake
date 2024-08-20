# Use whichever version of clang is the default for the OS
set(CMAKE_C_COMPILER clang-15)
set(CMAKE_CXX_COMPILER clang++-15)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
