#include "header.h"

int mvs_rectify_image_pair(
 char *filename1,
 char *filename2,
 char *filename_match,
 double max_mean_error
)

/*
The rectified images are named:
image1_rect.jpg
image2_rect.jpg
*/

/*
If rectification fails,
mvs_rectify_image_pair() returns 1
*/

/*
It is assumed sfm10 has been run prior to running mvs10
It means the matches have been stored on disk
using the sfm10 camera ordering
*/

{

 int err_flag;
 char filename1_rect[80];
 char filename2_rect[80];
 int width;
 int height;
 int *image1_arr;
 int *image2_arr;
 int width1;
 int height1;
 int width2;
 int height2;
 FILE *fp;

 /*
 Load image 1
 */

 err_flag= load_rgb_image(
  filename1,
  &image1_arr,
  &width1,
  &height1
 );

 free(image1_arr);

 /*
 Load image 2
 */

 err_flag= load_rgb_image(
  filename2,
  &image2_arr,
  &width2,
  &height2
 );

 free(image2_arr);

 if ( width2 != width1 ) {
    mvs10_error_handler("mvs_compute_depth_map");
 }
 if ( height2 != height1 ) {
    mvs10_error_handler("mvs_compute_depth_map");
 }

 width= width1;
 height= height1;

 /*
 Make sure the match file exists!
 */

 fp= fopen(filename_match,"r");

 if ( fp == 0 ) {
    mvs10_error_handler("mvs_rectify_image_pair");
 }

 /*
 Compute the homographies
 */

 fprintf(stdout,"Computing the rectifying homographies ...\n");

 err_flag= rectify_main(
  filename_match,
  width,
  height,
  "H1.txt",
  "H2.txt",
  "disp_range.txt",
  max_mean_error
 );

 fprintf(stdout,"Computing the rectifying homographies ... done.\n");

 /*
 Check if rectification failed
 */

 if ( err_flag == 1 ) {
    return 1;
 }

 /*
 Apply homography to image 1
 */

 fprintf(stdout,"Applying homography to image 1 ...\n");

 strcpy(filename1_rect,"image1_rect.jpg");
 err_flag= homography_main(
  filename1,
  "H1.txt",
  filename1_rect
 );

 fprintf(stdout,"Applying homography to image 1 ... done.\n");

 /*
 homography_main() should never fail
 */

 if ( err_flag == 1 ) {
    mvs10_error_handler("mvs_rectify_image_pair");
 }

 /*
 Apply homography to image 2
 */

 fprintf(stdout,"Applying homography to image 2 ...\n");

 strcpy(filename2_rect,"image2_rect.jpg");
 err_flag= homography_main(
  filename2,
  "H2.txt",
  filename2_rect
 );

 fprintf(stdout,"Applying homography to image 2 ... done.\n");

 /*
 homography_main() should never fail 
 */

 if ( err_flag == 1 ) {
    mvs10_error_handler("mvs_rectify_image_pair");
 }

 return 0;

}
