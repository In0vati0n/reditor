cmake_minimum_required(VERSION 3.1)

set(PROJ_NAME reditor)

project(${PROJ_NAME})

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

option(DEBUG "Debug Version" on)
message(STATUS "Debug Version ${DEBUG}")

if(WIN32 AND MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG /MTd")
elseif(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
else()
    if(DEBUG)
        add_definitions(-DDEBUG)
    endif(DEBUG)
endif()

option(EXPORT_COMPILE_COMMANDS "Export Compile Commands" on)
message(STATUS "Export Compile Commands ${EXPORT_COMPILE_COMMANDS}")
if (EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

set(PROJ_SOURCE_ROOT ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

# lua lib
add_subdirectory(libs/lua)

# rlib
set(RLIB_BUILD_TESTS OFF)
add_subdirectory(libs/rlib)

add_subdirectory(src)
add_subdirectory(test)
