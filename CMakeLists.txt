cmake_minimum_required (VERSION 3.8)

project( DisplayImage )


set(INCLUDES_PATH include)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(include)
add_library(Utils include/Utils.cpp include/Utils.h) 
add_library(LineFollower include/LineFollower.cpp include/LineFollower.h) 
add_executable( DisplayImage Hline.cpp )
target_link_libraries( Utils ${OpenCV_LIBS} ) 
target_link_libraries( LineFollower ${OpenCV_LIBS} Utils) 
target_link_libraries( DisplayImage ${OpenCV_LIBS} LineFollower) 

# find_package( OpenCV REQUIRED )
# include_directories( ${OpenCV_INCLUDE_DIRS} )
# include_directories(include) 
# add_executable( DisplayImage Record.cpp ) 
# target_link_libraries( DisplayImage ${OpenCV_LIBS}) 