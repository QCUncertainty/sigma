include_guard()

macro(cxx_api_docs)
    string(TOLOWER "${PROJECT_NAME}" _ncad_lc_name)
    set(_ncad_target "${_ncad_lc_name}_cxx_api")
    find_package(Doxygen REQUIRED)
    doxygen_add_docs("${_ncad_target}" ${ARGV})
endmacro()