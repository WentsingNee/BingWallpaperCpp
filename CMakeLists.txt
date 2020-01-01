cmake_minimum_required(VERSION 3.10)
project(BingWallpaperCpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(BingWallpaperCpp main.cpp)
target_link_libraries(BingWallpaperCpp -lcpr -lpthread)
