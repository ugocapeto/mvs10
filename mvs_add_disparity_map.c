#include "header.h"

void mvs_add_disparity_map(
 char **image_filename_arr,
 int *width_arr,
 int *height_arr,
 double *focal_length_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 char *filename1,
 char *filename2,
 int camera1_ind,
 int camera2_ind,
 int *disp1_arr,
 int *occ1_arr,
 int sampling_step,
 sfm_threedpoint_struct **pthreedpoint_arr,
 int *pthreedpoint_nbr,
 int ***ppixel2threedpoint_arr
)

{

 int err_flag;
 int *image1_arr;
 int width1;
 int height1;
 int *image2_arr;
 int width2;
 int height2;
 FILE *fp;
 int i;
 int j;
 double H1[3*3];
 double H2[3*3];
 double H1_inv[3*3];
 double H2_inv[3*3];
 int i1;
 int j1;
 int i1_rect;
 int j1_rect;
 int i2;
 int j2;
 int i2_rect;
 int j2_rect;
 double x1;
 double y1;
 double x1_rect;
 double y1_rect;
 double x2;
 double y2;
 double x2_rect;
 double y2_rect;
 int d1;
 sfm_threedpoint_struct *threedpoint_arr;
 int threedpoint_nbr;
 int **pixel2threedpoint_arr;
 int remainder= 0;

 threedpoint_arr= (*pthreedpoint_arr);
 threedpoint_nbr= (*pthreedpoint_nbr);
 pixel2threedpoint_arr= (*ppixel2threedpoint_arr);

 /*
 Load image 1
 */

 err_flag= load_rgb_image(
  filename1,
  &image1_arr,
  &width1,
  &height1
 );

 /*
 Load image 2
 */

 err_flag= load_rgb_image(
  filename2,
  &image2_arr,
  &width2,
  &height2
 );

 if ( width2 != width1 ) {
    mvs10_error_handler("mvs_compute_depth_map");
 }
 if ( height2 != height1 ) {
    mvs10_error_handler("mvs_compute_depth_map");
 }

 /*
 Get homography 1
 It is stored in file H1.txt
 */

 fp= fopen("H1.txt","r");

 for ( i= 0 ; i< 3 ; i++ ) {
    for ( j= 0 ; j< 3 ; j++ ) {
       fscanf(fp,"%lg",&H1[i*3+j]);
    }
 }

 fclose(fp);

 /*
 Invert homography 1
 */

 math_matrix3x3_inverse(
  H1,
  H1_inv
 );

 /*
 Get homography 2
 It is stored in file H2.txt
 */

 fp= fopen("H2.txt","r");

 for ( i= 0 ; i< 3 ; i++ ) {
    for ( j= 0 ; j< 3 ; j++ ) {
       fscanf(fp,"%lg",&H2[i*3+j]);
    }
 }

 fclose(fp);

 /*
 Invert homography 2
 */

 math_matrix3x3_inverse(
  H2,
  H2_inv
 );

 /*
 Process each pixel of unrectified image 1
 */

 for ( i1= 0 ; i1< height1 ; i1++ ) {
    if ( i1%sampling_step != remainder )
     continue;
    for ( j1= 0 ; j1< width1 ; j1++ ) {
       if ( j1%sampling_step != remainder )
        continue;

       x1= (double)j1;
       y1= (double)i1;

       /*
       Go from unrectified image to rectified image
       */

       homography_apply(
        H1,
        x1,
        y1,
        &x1_rect,
        &y1_rect
       );

       if ( x1_rect >= 0 )
        j1_rect= (int)(x1_rect+0.5);
       else 
        j1_rect= (int)(x1_rect-0.5);

       if ( y1_rect >= 0 )
        i1_rect= (int)(y1_rect+0.5);
       else
        i1_rect= (int)(y1_rect-0.5);

       /*
       Corresponding pixel in rectified image
       better be in bounds, otherwise ignore
       */

       if ( j1_rect < 0 )
        continue;
       if ( j1_rect > width1-1 )
        continue;

       if ( i1_rect < 0 )
        continue;
       if ( i1_rect > height1-1 )
        continue;

       /*
       Get the disparity
       */

       d1= disp1_arr[i1_rect*width1+j1_rect];

       /*
       Do not consider if the disparity is suspect
       */

       if ( occ1_arr[i1_rect*width1+j1_rect] == 0 )
        continue;

       /*
       Get the pixel in rectified image 2
       */

       i2_rect= i1_rect;
       j2_rect= j1_rect-d1;

       /*
       Matching pixel in rectified image 2
       better be in bounds, otherwise ignore
       */

       if ( j2_rect < 0 )
        continue;
       if ( j2_rect > width2-1 )
        continue;

       if ( i2_rect < 0 )
        continue;
       if ( i2_rect > height2-1 )
        continue;

       /*
       Go from rectified image to unrectified image
       */

       x2_rect= (double)j2_rect;
       y2_rect= (double)i2_rect;

       homography_apply(
        H2_inv,
        x2_rect,
        y2_rect,
        &x2,
        &y2
       );

       if ( x2 >= 0 )
        j2= (int)(x2+0.5);
       else
        j2= (int)(x2-0.5);

       if ( y2 >= 0 )
        i2= (int)(y2+0.5);
       else
        i2= (int)(y2-0.5);

       /*
       Corresponding pixel in unrectified image
       better be in bounds, otherwise ignore
       */

       if ( j2 < 0 )
        continue;
       if ( j2 > width2-1 )
        continue;

       if ( i2 < 0 )
        continue;
       if ( i2 > height2-1 )
        continue;

       /*
       Pixel (i1,j1) in image 1 matches
       pixel (i2,j2) in image 2
       */

       mvs_add_match(
        image_filename_arr,
        width_arr,
        height_arr,
        focal_length_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        filename1,
        filename2,
        image1_arr,
        image2_arr,
        camera1_ind,
        camera2_ind,
        i1, 
        j1,
        i2,
        j2,
        &threedpoint_arr,
        &threedpoint_nbr,
        &pixel2threedpoint_arr
       );
    }
 }

 free(image1_arr);

 free(image2_arr);

 (*pthreedpoint_arr)= threedpoint_arr;
 (*pthreedpoint_nbr)= threedpoint_nbr;
 (*ppixel2threedpoint_arr)= pixel2threedpoint_arr;

}
