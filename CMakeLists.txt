cmake_minimum_required(VERSION 2.8.11)
project(CALIB_CAM)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXE_LINKER_FLAGS -lcblas)

find_package(OpenCV REQUIRED)
include_directories($(OpenCV_INCLUDE_DIRS))

add_executable(calibrate calib_intrinsic.cpp)
target_link_libraries(calibrate ${OpenCV_LIBS})

add_executable(read read.cpp)
target_link_libraries(read ${OpenCV_LIBS})

add_executable(calibrate_stereo calib_stereo.cpp)
target_link_libraries(calibrate_stereo ${OpenCV_LIBS})

add_executable(undistort_rectify undistort_rectify.cpp ImageCorrection.cpp)
target_link_libraries(undistort_rectify ${OpenCV_LIBS})

add_executable(disparity disparity.cpp)
target_link_libraries(disparity ${OpenCV_LIBS})