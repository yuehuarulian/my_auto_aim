cmake_minimum_required(VERSION 3.22.1)

project(test)

find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)

include_directories(include)

# 每一个可执行文件都要add_executable一次，如下有matLearning、main两个可执行文件
# add_executable(
#     tongjiAD
#     src/interestADs.cpp
# )

add_executable(
    main
    src/main.cpp
    ##### 在下方添加所依赖的其他cpp文件，什么都不添加会报错！ #####
    src/detector.cpp
    src/camera.cpp
    src/classifier.cpp
    src/solvepnp.cpp
    src/getv.cpp
    #################################
)
# target_link_libraries(tongjiAD ${OpenCV_LIBS})
# target_link_libraries(matLearning ${OpenCV_LIBS})
target_link_libraries(main ${OpenCV_LIBS} fmt::fmt MVSDK)
