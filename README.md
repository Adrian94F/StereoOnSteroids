# Stereo On Steroids
My master thesis POC about possibilities of speeding up stereo vision (generating depth map) based on OpenCV library.

## How it works?
Save images pairs from camera by running
`./read`
and pressing any key. Press ESC key to exit.

calibrate left and right camera separately using
`./calibrate`
and calibrate camera system using
`./calibrate-stereo`.
Output files will generate: for each camera and set of cameras.

Then remove distortion and rectify images using
`./undistort_rectify`.
Output images with suffix "old" will appear.

Generate disparity map by running `./disparity`. It creates disparity map of live images from cameras and shows

<p align="center">
  <img src="./doc/screen1.png" width="500px")/>
  <br/>Example images from cameras with generated disparity map.
</p>

<p align="center">
  <img src="./doc/multi_2_wo_correction_result.png" width="500px")/>
  <br/>Example images with multithreaded generated disparity map.
</p>

<p align="center">
  <img src="./doc/multi_2_w_correction_result.png" width="500px")/>
  <br/>Example images with multithreaded generated disparity map (using black borders correction).
</p>

## Done
- Cameras set calibration
- Single- and multi-threaded version
  - Images collection from cameras
  - Fake camera for environment with no cameras connected
  - Disparity map generation
- Statistics printed at the end

## Tips
- on Manjaro Linux: build OpenCV with flag `CMAKE_EXE_LINKER_FLAGS=-lcblas` ([solution found here](https://unix.stackexchange.com/questions/417032/arch-linux-problems-building-opencv-with-cuda-libopencv-core-so-3-4-0-undefin)); added this flag to CMakeLists.txt
