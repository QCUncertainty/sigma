include_guard()

# Sets up documentation build process
macro(cxx_api_docs)
    # If ONLY_BUILD_DOCS is specified, then we are building the docs
    if("${ONLY_BUILD_DOCS}")
        set(BUILD_DOCS ON)
    endif()

    # Adding the docs target
    if("${BUILD_DOCS}")
    # Target naming
        string(TOLOWER "${PROJECT_NAME}" _ncad_lc_name)
        set(_ncad_target "${_ncad_lc_name}_cxx_api")

        # Find Doxygen
        find_package(Doxygen REQUIRED)

        # Doxygen Settinggs
        if("${ENABLE_EIGEN_SUPPORT}")
            set(DOXYGEN_PREDEFINED "ENABLE_EIGEN_SUPPORT")
        endif()
        set(DOXYGEN_MACRO_EXPANSION "YES")
        set(DOXYGEN_USE_MATHJAX "YES")
        set(DOXYGEN_MATHJAX_RELPATH "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5")
        
        # Get Awesome CSS Theme
        include(FetchContent)
        fetchcontent_declare(
            doxygen-awesome-css
            URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
        )
        fetchcontent_makeavailable(doxygen-awesome-css)
        fetchcontent_getproperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

        # Awesome CSS settings
        set(CSS_BASE "${AWESOME_CSS_DIR}/doxygen-awesome.css")
        set(CSS_SIDEBAR_ONLY "${AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only.css")
        set(CSS_DARKMODE_TOGGLE "${AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css")
        set(DOXYGEN_HTML_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/docs/header.html")
        set(DOXYGEN_HTML_EXTRA_FILES "${AWESOME_CSS_DIR}/doxygen-awesome-darkmode-toggle.js")
        set(DOXYGEN_GENERATE_TREEVIEW "YES")
        set(DOXYGEN_DISABLE_INDEX "NO")
        set(DOXYGEN_FULL_SIDEBAR "NO")
        set(DOXYGEN_HTML_EXTRA_STYLESHEET "${CSS_BASE}" "${CSS_SIDEBAR_ONLY}" "${CSS_DARKMODE_TOGGLE}")
        set(DOXYGEN_HTML_COLORSTYLE "LIGHT")

        # Add the Doxygen target
        doxygen_add_docs("${_ncad_target}" ${ARGV})

        # If we're only building docs, then we're done
        if("${ONLY_BUILD_DOCS}")
            return()
        endif()
    endif()
endmacro()
