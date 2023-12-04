cmake_minimum_required(VERSION 3.22.1)

project(Week1)

include_directories(include)
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(
    main 
    src/main.cpp)

target_link_libraries(main fmt::fmt Eigen3::Eigen ${OpenCV_LIBS} )  #  MVSDK)