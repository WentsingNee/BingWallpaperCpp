cmake_minimum_required(VERSION 3.10)
project(BingWallpaperCpp CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(BingWallpaperCpp main.cpp)

if (${BING_WALLPAPER_CPP_USE_STATIC_LIBRARY_PRIOR})
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
endif()


message(STATUS "Finding cpr ...")
find_library(LIB_CPR cpr REQUIRED)
if (LIB_CPR)
    message(STATUS "Found cpr: ${LIB_CPR}")
else ()
    message(FATAL_ERROR "cpr not found")
endif ()


message(STATUS "Finding sqlite3 ...")
find_library(LIB_SQLITE3 sqlite3 REQUIRED)
if (LIB_SQLITE3)
    message(STATUS "Found sqlite3: ${LIB_SQLITE3}")
else ()
    message(FATAL_ERROR "sqlite3 not found")
endif ()


target_link_libraries(BingWallpaperCpp PUBLIC ${LIB_CPR})
target_link_libraries(BingWallpaperCpp PUBLIC ${LIB_SQLITE3})


add_executable(log2sqlite log2sqlite.cpp)
target_link_libraries(log2sqlite PUBLIC ${LIB_SQLITE3})


if (${BING_WALLPAPER_CPP_USE_STATIC_LIBRARY_PRIOR})
    set(CMAKE_FIND_LIBRARY_SUFFIXES .so ${CMAKE_FIND_LIBRARY_SUFFIXES})


    message(STATUS "Finding curl ...")
    find_library(LIB_CURL curl REQUIRED)
    if (LIB_CURL)
        message(STATUS "Found curl: ${LIB_CURL}")
    else ()
        message(FATAL_ERROR "curl not found")
    endif ()
    target_link_libraries(BingWallpaperCpp PUBLIC ${LIB_CURL})


    message(STATUS "Finding ssl ...")
    find_library(LIB_OPENSSL ssl REQUIRED)
    if (LIB_OPENSSL)
        message(STATUS "Found ssl: ${LIB_OPENSSL}")
    else ()
        message(FATAL_ERROR "ssl not found")
    endif ()
    target_link_libraries(BingWallpaperCpp PUBLIC ${LIB_OPENSSL})


endif()
