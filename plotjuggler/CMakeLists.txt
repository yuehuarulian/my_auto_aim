cmake_minimum_required(VERSION 3.22.1)

project(Week1)

include_directories(include)
find_package(fmt REQUIRED)

add_executable(
    main 
    src/main.cpp)

target_link_libraries(main fmt::fmt)# ${OpenCV_LIBS}  MVSDK)