cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

# Windows Specifics
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Set Project and Version
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(
    sigma
    VERSION "${VERSION}"
    LANGUAGES CXX
)

# Paths
set(${PROJECT_NAME}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(${PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(${PROJECT_NAME}_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(${PROJECT_NAME}_DOCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")

# Get CMaize
include(cmake/get_cmaize.cmake)

# Options
cmaize_option_list(
    BUILD_TESTING OFF "Should the tests be built?"
    BUILD_DOCS OFF "Should we build the documentation?"
    ONLY_BUILD_DOCS OFF "Should we only build the documentation?"
    DOCS_FAIL_ON_WARNING OFF
    "Should the documentation build fail from warnings?"
    ENABLE_EIGEN_SUPPORT ON "Include Eigen compatibility headers?"
    BUILD_PYBIND11_BINDINGS OFF "Should we build the Python bindings?"
)

# Docs
include(cmake/cxx_api_docs.cmake)
cxx_api_docs()

# Dependencies
find_package(Boost REQUIRED)

cmaize_find_or_build_optional_dependency(
    eigen ENABLE_EIGEN_SUPPORT
    NAME Eigen3
    URL https://www.gitlab.com/libeigen/eigen
    VERSION 5e4f3475b5cb77414bca1f3dde7d6fd9cb4d2011
    BUILD_TARGET eigen
    FIND_TARGET Eigen3::Eigen
    CMAKE_ARGS EIGEN_BUILD_TESTING=OFF
)

# Add libraries
cmaize_add_library(
    ${PROJECT_NAME}
    INCLUDE_DIRS "${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME}"
    DEPENDS eigen Boost::boost
)

# boost::numeric::interval, which sigma::Interval wraps, gets its outward
# rounding by changing the FPU's rounding mode around each operation. A compiler
# is allowed to assume the mode never changes, and clang does exactly that:
# without this flag it folds the directed operations back to round-to-nearest
# and the computed bounds silently stop enclosing the true result. Sigma is
# header-only, so the flag has to reach every translation unit that uses it.
if(MSVC)
    target_compile_options(${PROJECT_NAME} INTERFACE /fp:strict)
else()
    target_compile_options(${PROJECT_NAME} INTERFACE -frounding-math)
endif()

# Add ENABLE_EIGEN_SUPPORT compile definition to the main target, since it does
# not appear to be propagated from the eigen target. This is necessary for the
# compile definition to appear in the Sigma installation properly for
# dependents.
if("${ENABLE_EIGEN_SUPPORT}")
    cpp_get_global(_top_proj CMAIZE_TOP_PROJECT)
    CMaizeProject(
        get_target
        "${_top_proj}"
        _main_tgt ${PROJECT_NAME}
    )
    CMaizeTarget(
        set_property
        "${_main_tgt}"
        INTERFACE_COMPILE_DEFINITIONS "ENABLE_EIGEN_SUPPORT"
    )
endif()

# Python bindings
if("${BUILD_PYBIND11_BINDINGS}")
    # pybind11 v3 builds extensions via python_add_library, which comes from
    # FindPython's Development.Module component.
    find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

    # Keep this tag in sync with NWXCMake's cmake/dependencies/pybind11.cmake.
    # pybind11 keys its type registry on PYBIND11_INTERNALS_VERSION, so sigma's
    # types can only be passed to and from the NWChemEx modules if every module
    # is built against the same pybind11.
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG v3.1.0
    )
    FetchContent_MakeAvailable(pybind11)

    file(GLOB_RECURSE _sigma_python_sources CONFIGURE_DEPENDS
         "${${PROJECT_NAME}_SOURCE_DIR}/python/export_*.cpp"
    )
    pybind11_add_module(${PROJECT_NAME}_python ${_sigma_python_sources})
    # Linking the sigma target is what gives the module the rounding-mode flag
    # (see above), without which the bounds it computes are not enclosures.
    target_link_libraries(${PROJECT_NAME}_python PRIVATE ${PROJECT_NAME})
    set_target_properties(
        ${PROJECT_NAME}_python PROPERTIES OUTPUT_NAME ${PROJECT_NAME}
    )
endif()

# Build tests
if("${BUILD_TESTING}")
    # Find or build dependencies for tests
    cmaize_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
        VERSION v3.6.0
    )

    # Add Tests
    cmaize_add_tests(
        test_${PROJECT_NAME}
        SOURCE_DIR "${${PROJECT_NAME}_TESTS_DIR}/unit_tests"
        DEPENDS Catch2 eigen ${PROJECT_NAME}
    )

    if("${BUILD_PYBIND11_BINDINGS}")
        add_test(
            NAME py_${PROJECT_NAME}
            COMMAND
                "${Python_EXECUTABLE}"
                "${${PROJECT_NAME}_TESTS_DIR}/python/unit_tests/test_${PROJECT_NAME}.py"
        )
        set_tests_properties(
            py_${PROJECT_NAME}
            PROPERTIES ENVIRONMENT
                       "PYTHONPATH=$<TARGET_FILE_DIR:${PROJECT_NAME}_python>"
        )
    endif()
endif()

# Add package
cmaize_add_package(${PROJECT_NAME} NAMESPACE ${PROJECT_NAME}::)
