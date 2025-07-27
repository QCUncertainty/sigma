cmake_minimum_required(VERSION 3.14)

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
)

## Docs ##
include(cmake/cxx_api_docs.cmake)

## Dependencies ##
cmaize_find_or_build_optional_dependency(
    eigen
    ENABLE_EIGEN_SUPPORT
    NAME Eigen3
    URL https://www.gitlab.com/libeigen/eigen
    VERSION 2e76277bd049f7bec36b0f908c69734a42c5234f
    BUILD_TARGET eigen
    FIND_TARGET Eigen3::Eigen
    CMAKE_ARGS EIGEN_BUILD_TESTING=OFF
)

## Add libraries ##
cmaize_add_library(
    ${PROJECT_NAME}
    # SOURCE_DIR "${${PROJECT_NAME}_SOURCE_DIR}/${PROJECT_NAME}"
    INCLUDE_DIRS "${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME}"
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
if(ENABLE_EIGEN_SUPPORT)
    target_compile_definitions(${PROJECT_NAME} INTERFACE ENABLE_EIGEN_SUPPORT)
endif()
# set_target_properties( ${PROJECT_NAME} PROPERTIES INTERFACE_LINK_LIBRARIES
# Eigen3::Eigen )
target_link_libraries(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:Eigen3::Eigen>")

# Build tests ##
if("${BUILD_TESTING}")
    # Find or build dependencies for tests
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
        # INCLUDE_DIRS "${${PROJECT_NAME}_SOURCE_DIR}/${PROJECT_NAME}"
        DEPENDS Catch2 eigen ${PROJECT_NAME}
    )
endif()

# Install package ## CMaize does not properly handle interface libraries
# cmaize_add_package( ${PROJECT_NAME} NAMESPACE ${PROJECT_NAME}:: )
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake
)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
        DESTINATION include
)
