cmake_minimum_required(VERSION 2.8.11)
project(StereoOnSteroids)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXE_LINKER_FLAGS -lcblas)

find_package(OpenCV REQUIRED)
include_directories($(OpenCV_INCLUDE_DIRS)/opencv4)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.58.0 COMPONENTS system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})

    add_executable(disparity disparity.cpp ImageCorrection.cpp DisparityMapCalculator.cpp ICameras.cpp Cameras.cpp FakeCameras.cpp Timer.cpp)
    target_link_libraries(disparity ${OpenCV_LIBS} ${Boost_LIBRARIES} -lrt -lpthread)
endif()

add_executable(calibrate calib_intrinsic.cpp)
target_link_libraries(calibrate ${OpenCV_LIBS})

add_executable(read read.cpp ICameras.cpp Cameras.cpp FakeCameras.cpp)
target_link_libraries(read ${OpenCV_LIBS})

add_executable(calibrate_stereo calib_stereo.cpp)
target_link_libraries(calibrate_stereo ${OpenCV_LIBS})

add_executable(undistort_rectify undistort_rectify.cpp ImageCorrection.cpp)
target_link_libraries(undistort_rectify ${OpenCV_LIBS})