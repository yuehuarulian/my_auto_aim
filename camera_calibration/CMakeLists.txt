cmake_minimum_required(VERSION 3.22.1)

project(Week1)

find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

add_executable(
    main 
    src/main.cpp
    src/calibratecamera.cpp
    )

target_link_libraries(main fmt::fmt Eigen3::Eigen ${OpenCV_LIBS} MVSDK)  #  )