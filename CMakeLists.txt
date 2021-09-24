cmake_minimum_required (VERSION 3.8)

project( DisplayImage )


set(INCLUDES_PATH include)
# find_package( OpenCV REQUIRED ) 
find_package(OpenCV REQUIRED PATHS /usr/local/opt/opencv@3 NO_DEFAULT_PATH)
include_directories(include)
# include_directories( PATHS /usr/local/opt/opencv@3/include NO_DEFAULT_PATH)
include_directories(${OpenCV_INCLUDE_DIRS})
add_library(Utils include/Utils.cpp include/Utils.h) 
add_library(BlueRect include/BlueRect.cpp include/BlueRect.h) 
add_executable( DisplayImage biru.cpp ) 
target_link_libraries( Utils ${OpenCV_LIBS} ) 
target_link_libraries( BlueRect ${OpenCV_LIBS} Utils) 
target_link_libraries( DisplayImage ${OpenCV_LIBS} BlueRect) 
 
# find_package( OpenCV REQUIRED )
# include_directories( ${OpenCV_INCLUDE_DIRS} )
# include_directories(include) 
# add_executable( DisplayImage Record.cpp ) 
# target_link_libraries( DisplayImage ${OpenCV_LIBS}) 