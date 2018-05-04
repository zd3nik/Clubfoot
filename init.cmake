if (NOT CMAKE_INITIALIZED)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_STANDARD 11)
    endif()

    file(STRINGS "${CMAKE_SOURCE_DIR}/.git/refs/heads/master" GIT_REV)
    message("-- GIT_REV ${GIT_REV}")

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "-DGIT_REV=${GIT_REV} -fPIC -m64 -Wall -Wextra -Wpedantic ${CMAKE_CXX_FLAGS}")
    endif()

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_INITIALIZED 1)
endif()

