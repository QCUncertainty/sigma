include_guard()

# Sets up documentation build process
macro(cxx_api_docs)
    # If ONLY_BUILD_DOCS is specified, then we are building the docs
    if("${ONLY_BUILD_DOCS}")
        set(BUILD_DOCS ON)
    endif()

    # Adding the docs target
    if("${BUILD_DOCS}")
        # Find Doxygen
        find_package(Doxygen 1.10.0 REQUIRED)

        # Target naming
        string(TOLOWER "${PROJECT_NAME}" _ncad_lc_name)
        set(_ncad_target "${_ncad_lc_name}_cxx_api")

        # Get Doxygen Awesome CSS Theme
        include(FetchContent)
        fetchcontent_declare(
            doxygen-awesome-css
            URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
        )
        fetchcontent_makeavailable(doxygen-awesome-css)

        # Set Paths
        fetchcontent_getproperties(doxygen-awesome-css SOURCE_DIR THEME_DIR)
        set(CUSTOM_DIR "${${PROJECT_NAME}_DOCS_DIR}/doxygen-custom")

        # Doxygen Settinggs
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
        set(DOXYGEN_GENERATE_TREEVIEW "YES")
        set(DOXYGEN_DISABLE_INDEX "NO")
        set(DOXYGEN_FULL_SIDEBAR "NO")
        set(DOXYGEN_MACRO_EXPANSION "YES")
        set(DOXYGEN_MARKDOWN_ID_STYLE "GITHUB")
        set(DOXYGEN_USE_MATHJAX "YES")
        set(DOXYGEN_MATHJAX_RELPATH "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5")
        set(DOXYGEN_HTML_COLORSTYLE "LIGHT")
        set(DOXYGEN_HTML_HEADER "${CUSTOM_DIR}/header.html")
        set(DOXYGEN_HTML_EXTRA_FILES "${THEME_DIR}/doxygen-awesome-darkmode-toggle.js")
        set(DOXYGEN_HTML_EXTRA_STYLESHEET "${THEME_DIR}/doxygen-awesome.css" 
                                          "${THEME_DIR}/doxygen-awesome-sidebar-only.css" 
                                          "${THEME_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css"
                                          "${CUSTOM_DIR}/custom.css")
        if("${ENABLE_EIGEN_SUPPORT}")
            set(DOXYGEN_PREDEFINED "ENABLE_EIGEN_SUPPORT")
        endif()
        if("${DOCS_FAIL_ON_WARNING}")
            set(DOXYGEN_WARN_AS_ERROR "FAIL_ON_WARNINGS")
        endif()

        # Add the Doxygen target
        doxygen_add_docs("${_ncad_target}" 
                         "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
                         "${${PROJECT_NAME}_INCLUDE_DIR}"
                         "${${PROJECT_NAME}_DOCS_DIR}")

        # If we're only building docs, then we're done
        if("${ONLY_BUILD_DOCS}")
            return()
        endif()
    endif()
endmacro()
