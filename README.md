# Stereo On Steroids
My master thesis POC about possibilities of speeding up stereo vision (generating depth map) based on OpenCV library.

# How it works?
Save images pairs from camera by running
`./read`
and pressing any key. Press ESC key to exit.

calibrate left and right camera separetly using
`./calibrate`
and calibrate camera system using
`./calibrate-stereo'.
Output files will generate: for each camera and set of cameras.

Then remove distortion and rectify images using
`./undistort_rectify`.
Output images with suffix "old" will appear.

## TODO
- Disparity map generation
- Speeding up

## Done
- Collecting images from cameras
- Calibration
