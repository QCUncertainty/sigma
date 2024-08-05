cmake_minimum_required(VERSION 3.14)

## Set Project and Version
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(sigma VERSION "${VERSION}" LANGUAGES CXX)

## Get CMaize
include(cmake/get_cmaize.cmake)
include(cmake/cxx_api_docs.cmake)

## Paths ##
set(${PROJECT_NAME}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(${PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(${PROJECT_NAME}_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

## Options ##
cmaize_option_list(
    BUILD_TESTING OFF "Should the tests be built?"
    BUILD_DOCS OFF "Should we build the documentation?"
)

cmaize_find_or_build_dependency(
    eigen
    URL https://www.gitlab.com/libeigen/eigen
    VERSION 3.4.0
    BUILD_TARGET eigen
    FIND_TARGET Eigen3::Eigen
)

## Add libraries ##
cmaize_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${${PROJECT_NAME}_SOURCE_DIR}/${PROJECT_NAME}"
    INCLUDE_DIRS "${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME}"
    DEPENDS eigen
)

if("${BUILD_DOCS}")
    cxx_api_docs("${${PROJECT_NAME}_SOURCE_DIR}" "${${PROJECT_NAME}_INCLUDE_DIR}") 
endif()

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