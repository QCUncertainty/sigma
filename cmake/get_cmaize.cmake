function(get_cmaize)

    if("${CMAIZE_VERSION}" STREQUAL "")
        set(CMAIZE_VERSION v1.1.0 )
    endif()

    # Store whether we are building tests or not, then turn off the tests
    if(BUILD_TESTING)
        set(build_testing_old "${BUILD_TESTING}")
    endif()
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

    # Download CMakePP and bring it into scope
    include(FetchContent)
    FetchContent_Declare(
        cmaize
        GIT_REPOSITORY https://github.com/CMakePP/CMaize
        GIT_TAG ${CMAIZE_VERSION}
    )
    FetchContent_MakeAvailable(cmaize)

    # Restore the previous value, if set
    # Unset otherwise
    if(build_testing_old)
        set(BUILD_TESTING "${build_testing_old}" CACHE BOOL "" FORCE)
    else()
        unset(BUILD_TESTING CACHE)
    endif()
endfunction()

# Call the function we just wrote to get CMaize
get_cmaize()

# Include CMaize
include(cmaize/cmaize)