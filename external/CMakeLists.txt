include(FetchContent)

# ==============================================================================
# hmem
# ==============================================================================
if(NOT TARGET hmem::hmem)
    find_package(hmem QUIET)

    if(NOT TARGET hmem::hmem)
        message(STATUS "Fetching hmem dependency")

        FetchContent_Declare(
            hmem
            GIT_REPOSITORY
                https://github.com/CodeCin523/hmem.git
            GIT_TAG
                main
        )

        FetchContent_MakeAvailable(hmem)
    else()
        message(STATUS "Using system-installed hmem")
    endif()
else()

    message(STATUS "Using existing hmem target")
endif()