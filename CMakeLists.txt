cmake_minimum_required(VERSION 3.1.0)

project("subhook-trampoline-test")
enable_language("CXX")

add_executable("test" "src/test.cxx")

function(find_subhook)
    if(NOT SUBHOOK_INCLUDE_PATH)
        unset("SUBHOOK_HEADER_PATH" CACHE)
        find_file("SUBHOOK_HEADER_PATH" "subhook.h")
        mark_as_advanced("SUBHOOK_HEADER_PATH")
        if(SUBHOOK_HEADER_PATH)
            unset("SUBHOOK_INCLUDE_PATH" CACHE)
            get_filename_component("SUBHOOK_INCLUDE_PATH" "${SUBHOOK_HEADER_PATH}" DIRECTORY)
            set("SUBHOOK_INCLUDE_PATH" "${SUBHOOK_INCLUDE_PATH}" PARENT_SCOPE)
            mark_as_advanced("SUBHOOK_INCLUDE_PATH")
        else(SUBHOOK_HEADER_PATH)
            message(FATAL_ERROR "failed to find subhook.h")
        endif(SUBHOOK_HEADER_PATH)
    endif(NOT SUBHOOK_INCLUDE_PATH)

    if(NOT SUBHOOK_LIBRARY_PATH)
        unset("SUBHOOK_LIBRARY_PATH" CACHE)
        find_library("SUBHOOK_LIBRARY_PATH" "subhook")
        mark_as_advanced("SUBHOOK_LIBRARY_PATH")
        if(SUBHOOK_LIBRARY_PATH)
            set("SUBHOOK_LIBRARY_PATH" "${SUBHOOK_LIBRARY_PATH}" PARENT_SCOPE)
        else(SUBHOOK_LIBRARY_PATH)
            message(FATAL_ERROR "failed to find subhook library file")
        endif(SUBHOOK_LIBRARY_PATH)
    endif(NOT SUBHOOK_LIBRARY_PATH)
endfunction(find_subhook)

find_subhook()
target_include_directories("test" PRIVATE "${SUBHOOK_INCLUDE_PATH}")
target_link_libraries("test" PRIVATE "${SUBHOOK_LIBRARY_PATH}")
target_link_libraries("test" PRIVATE "foo")

add_library("foo" SHARED "src/foo.cxx" "src/foo.hpp" "src/symspec.h")

set_property(TARGET "foo" PROPERTY C_VISIBILITY_PRESET "hidden")
set_property(TARGET "foo" PROPERTY CXX_VISIBILITY_PRESET "hidden")
set_property(TARGET "foo" PROPERTY VISIBILITY_INLINES_HIDDEN TRUE)
set_property(TARGET "foo" PROPERTY DEFINE_SYMBOL "FOO_IMPLEMENTATION")

set_target_properties("test" "foo" PROPERTIES
    CXX_STANDARD 17
    POSITION_INDEPENDENT_CODE TRUE
)
