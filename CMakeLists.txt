cmake_minimum_required(VERSION 3.14)

# # Windows Specifics
if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif (WIN32)

## Set Project and Version
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(sigma VERSION "${VERSION}" LANGUAGES CXX)

## Paths ##
set(${PROJECT_NAME}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(${PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(${PROJECT_NAME}_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(${PROJECT_NAME}_DOCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")

## Get CMaize
include(cmake/get_cmaize.cmake)

## Options ##
cmaize_option_list(
    BUILD_TESTING OFF "Should the tests be built?"
    BUILD_DOCS OFF "Should we build the documentation?"
    ONLY_BUILD_DOCS OFF "Should we only build the documentation?"
    DOCS_FAIL_ON_WARNING OFF "Should the documentation build fail from warnings?"
    ENABLE_EIGEN_SUPPORT ON "Include Eigen compatibility headers?"
    # HEADER_ONLY ON "Header only or not"
)

## Docs ##
include(cmake/cxx_api_docs.cmake)

## Dependencies ##
cmaize_find_or_build_optional_dependency(
    eigen
    ENABLE_EIGEN_SUPPORT
    NAME Eigen3
    URL https://www.gitlab.com/libeigen/eigen
    VERSION 5e4f3475b5cb77414bca1f3dde7d6fd9cb4d2011
    BUILD_TARGET eigen
    FIND_TARGET Eigen3::Eigen
    CMAKE_ARGS EIGEN_BUILD_TESTING=OFF
)

## Add libraries ##
# if("${HEADER_ONLY}")
    cmaize_add_library(
        ${PROJECT_NAME}
        INCLUDE_DIRS "${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME}"
        DEPENDS eigen
    )
    if("${ENABLE_EIGEN_SUPPORT}")
        cpp_get_global(_top_proj CMAIZE_TOP_PROJECT)
        CMaizeProject(get_target "${_top_proj}" _main_tgt ${PROJECT_NAME})
        CMaizeTarget(set_property "${_main_tgt}" INTERFACE_COMPILE_DEFINITIONS "ENABLE_EIGEN_SUPPORT")
    endif()
# else()
#     cmaize_add_library(
#         ${PROJECT_NAME}
#         SOURCE_DIR "${${PROJECT_NAME}_SOURCE_DIR}/${PROJECT_NAME}"
#         INCLUDE_DIRS "${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME}"
#         DEPENDS eigen
#     )
# endif()

## Build tests ##
if("${BUILD_TESTING}")
    ## Find or build dependencies for tests
    cmaize_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
        VERSION v3.6.0
    )

    ## Add Tests ##
    cmaize_add_tests(
        test_${PROJECT_NAME}
        SOURCE_DIR "${${PROJECT_NAME}_TESTS_DIR}/unit_tests"
        INCLUDE_DIRS "${${PROJECT_NAME}_SOURCE_DIR}/${PROJECT_NAME}"
        DEPENDS Catch2 eigen ${PROJECT_NAME}
    )

endif()

## Add package ##
cmaize_add_package(
    ${PROJECT_NAME} NAMESPACE ${PROJECT_NAME}::
)