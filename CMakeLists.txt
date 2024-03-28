cmake_minimum_required(VERSION 3.10)
project(BingWallpaperCpp CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(BingWallpaperCpp main.cpp)

foreach (lib IN ITEMS cpr fmt Kerbal nlohmann_json unofficial-sqlite3)
    message(STATUS "Finding ${lib} ...")
    find_package(${lib} REQUIRED)
    message(STATUS "Finding ${lib} -- Found")
endforeach ()


target_link_options(
        BingWallpaperCpp
        PRIVATE
            -static-libstdc++
)
target_link_libraries(
        BingWallpaperCpp
        PRIVATE
            cpr::cpr
            fmt::fmt
            Kerbal::kerbal
            nlohmann_json::nlohmann_json
            unofficial::sqlite3::sqlite3
)


add_executable(log2sqlite log2sqlite.cpp)
target_link_libraries(log2sqlite PRIVATE unofficial::sqlite3::sqlite3)

