cmake_minimum_required(VERSION 2.8)

#-----------------------------------------------------------------------------
# Build senjo lib
#
# Ordinarily this would be done local to the senjo source directory.
# But since senjo is a git submodule I don't want to add a CMakeLists.txt
# to it because it may conflict with the cmake setup of any project that
# wishes to use it.
#
# So I'll just do everything here in this top-level CMakeLists.txt
#-----------------------------------------------------------------------------
project(senjo CXX)
include(init.cmake)
set(OBJ_HDR
    senjo/src/BackgroundCommand.h
    senjo/src/ChessMove.h
    senjo/src/MoveFinder.h
    senjo/src/Platform.h
    senjo/src/Threading.h
    senjo/src/ChessEngine.h
    senjo/src/EngineOption.h
    senjo/src/Output.h
    senjo/src/Square.h
    senjo/src/UCIAdapter.h
)
set(OBJ_SRC
    senjo/src/BackgroundCommand.cpp
    senjo/src/ChessEngine.cpp
    senjo/src/EngineOption.cpp
    senjo/src/MoveFinder.cpp
    senjo/src/Output.cpp
    senjo/src/Threading.cpp
    senjo/src/UCIAdapter.cpp
)

include_directories(senjo/src)
add_library(${PROJECT_NAME} STATIC ${OBJ_HDR} ${OBJ_SRC})
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

#-----------------------------------------------------------------------------
# Build clubfoot
#
# This would ordinarily be done in a CMakeLists.txt local to the clubfoot
# source directory.  See comment above, in the build senjo section.
#-----------------------------------------------------------------------------
project(clubfoot CXX)
include(init.cmake)
set(OBJ_HDR
    src/ClubFoot.h
    src/HashTable.h
    src/Move.h
    src/Stats.h
    src/Types.h
)
set(OBJ_SRC
    src/ClubFoot.cpp
    src/HashTable.cpp
    src/Stats.cpp
)

include_directories(src senjo/src .)
add_executable(${PROJECT_NAME} ${OBJ_HDR} ${OBJ_SRC} src/main.cpp)
target_link_libraries(${PROJECT_NAME} senjo)

add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/epd" "${CMAKE_BINARY_DIR}/epd"
    COMMENT "Copying EPD test files"
)

