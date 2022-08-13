# mvs10

Written and tested on Ubuntu 22.04.

mvs10 builds a dense reconstruction of a 3D scene from a set of pictures.

mvs10's workflow is as follows:
- Initialize the dense with 3D reconstruction with the sparse 3D reconstruction contained in the nvm file
- For each pair of images/cameras
-- Rectify the images using code similar to Epipolar Rectification 9b (ER9b)
-- Compute the disparity map using code similar to Depth Map Automatic Generator 5 (DMAG5)
-- Add the matches (pairs of image points) coming from the disparity map to the 3D reconstruction
-- Remove image points for which the reprojection error is too large
- Remove 3D points (also known as tracks in the literature) for which the number of image points is too low

To create the executable, compile the code in directory "mvs10" using "make -f Makefile_g/Makefile_O" and then go into the "main" directory and create the exec using "make".

Test cases are given in the "test" directory under main. Only the input file mvs10_input.txt is provided since mvs10 needs the output of sfm10 as input. For a given test case, copy all the files in the sfm10 corresponding directory (after sfm10 has been run, of course) and then run mvs10.

Info about mvs10 (theory behind it and how to use it) can be found here:

[Multi View Stereo 10 (MVS10)](http://3dstereophoto.blogspot.com/2016/04/multi-view-stereo-10-mvs10.html)

Dependencies (check the Makefile in main):

common repo

dmag5 repo

er9b repo

sfm10 repo
