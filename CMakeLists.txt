cmake_minimum_required (VERSION 3.8)

project( DisplayImage )


set(INCLUDES_PATH include)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(include)
add_library(Utils include/Utils.cpp include/Utils.h) 
add_executable( DisplayImage LineDetectABC.cpp )
target_link_libraries( Utils ${OpenCV_LIBS} ) 
target_link_libraries( DisplayImage ${OpenCV_LIBS} Utils) 