cmake_minimum_required(VERSION 2.8)
project( fish_heart_rate )
find_package( OpenCV )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( fish_heart_rate  fish_heart_rate.cpp helper.cpp )
target_link_libraries( fish_heart_rate ${OpenCV_LIBS} )