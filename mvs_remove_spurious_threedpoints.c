#include "header.h"

void mvs_remove_spurious_threedpoints(
 char **image_filename_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct **pthreedpoint_arr,
 int *pthreedpoint_nbr,
 double min_separation_angle,
 int min_imagepoint_nbr
)

/*
We're gonna rebuild threedpoint_arr,
delete the passed threedpoint_arr, and
return the newly built threedpoint_arr
*/

/*
threedpoint_arr is passed by reference
because it's gonna be replaced
*/

/*
If pixel2threedpoint_arr is needed,
it will need to be rebuilt with mvs_init_pixel_to_threedpoint_arr()
*/

{

 int threedpoint_ind;
 double xyz[3];
 int rgb[3];
 sfm_imagepoint_struct *imagepoint_arr;
 int imagepoint_nbr;
 sfm_threedpoint_struct *inp_threedpoint_arr;
 int inp_threedpoint_nbr;
 sfm_threedpoint_struct *threedpoint_arr;
 int threedpoint_nbr;
 int camera_ind;
 int feat_ind;
 int inp_threedpoint_ind;
 sfm_imagepoint_struct *inp_imagepoint_arr;
 int err_flag;
 int imagepoint_ind;
 sfm_imagepoint_struct imagepoint;
 double x;
 double y;
 int count1;
 int count2;
 int count3;
 int count0;
 int count0b;

 inp_threedpoint_arr= *pthreedpoint_arr;
 inp_threedpoint_nbr= *pthreedpoint_nbr;

 /*
 Initialize the 3D points
 */

 threedpoint_arr= 0;
 threedpoint_nbr= 0; 

 /*
 Loop through the current 3D points
 */

 count0= 0;
 count0b= 0;
 count1= 0;
 count2= 0;
 count3= 0;

 for ( inp_threedpoint_ind= 0 ;
       inp_threedpoint_ind< inp_threedpoint_nbr ;
       inp_threedpoint_ind++ ) {

    xyz[0]= inp_threedpoint_arr[inp_threedpoint_ind].xyz[0];
    xyz[1]= inp_threedpoint_arr[inp_threedpoint_ind].xyz[1];
    xyz[2]= inp_threedpoint_arr[inp_threedpoint_ind].xyz[2];
    rgb[0]= inp_threedpoint_arr[inp_threedpoint_ind].rgb[0];
    rgb[1]= inp_threedpoint_arr[inp_threedpoint_ind].rgb[1];
    rgb[2]= inp_threedpoint_arr[inp_threedpoint_ind].rgb[2];
    inp_imagepoint_arr= inp_threedpoint_arr[inp_threedpoint_ind].imagepoint_arr;
    imagepoint_nbr= inp_threedpoint_arr[inp_threedpoint_ind].imagepoint_nbr;

    /*
    Check for 3D point that doesn't actually exist
    (0 image point)
    */

    if ( imagepoint_nbr == 0 ) {
       if ( inp_imagepoint_arr != 0 ) {
          mvs10_error_handler("mvs_remove_spurious_threedpoints");
       }

       /*
       Do not keep this 3D point
       */

       count0++;
       continue;
    }

    /*
    Reject any 3D point that has 1 image point
    */

    if ( imagepoint_nbr == 1 ) {

       /*
       Do not keep this 3D point
       */

       count0b++;
       continue;
    }

    /*
    Reject any 3D point that's behind the camera
    */

    if ( !(xyz[2] > 0) ) {

       /*
       Do not keep this 3D point
       */

       count1++;
       continue;
    }

    /*
    Reject any 3D point with a separation angle that's too small
    */

    err_flag= sfm_separation_angle_threedpoint(
     image_filename_arr,
     camera_nbr,
     camera_matrix_arr,
     camera_matrix_nbr,
     inp_threedpoint_arr,
     inp_threedpoint_nbr,
     inp_threedpoint_ind,
     min_separation_angle
    );

    if ( err_flag == 1 ) {

       /*
       Do not keep this 3D point
       */

       count2++;
       continue;
    }

    /*
    Reject any 3D point with too few image points
    */

    if ( imagepoint_nbr < min_imagepoint_nbr ) {

       /*
       Do not keep this 3D point
       */

       count3++;
       continue;
    }

    /*
    Store the 3D point in threedpoint_arr
    */

    if ( threedpoint_nbr == 0 ) {
       threedpoint_arr= (sfm_threedpoint_struct *)calloc(threedpoint_nbr+1,sizeof(sfm_threedpoint_struct));
    }
    else {
       threedpoint_arr= (sfm_threedpoint_struct *)realloc(threedpoint_arr,(threedpoint_nbr+1)*sizeof(sfm_threedpoint_struct));
    }
    threedpoint_ind= threedpoint_nbr;
    threedpoint_nbr++;

    imagepoint_arr= (sfm_imagepoint_struct *)calloc(imagepoint_nbr,sizeof(sfm_imagepoint_struct));

    for ( imagepoint_ind= 0 ;
          imagepoint_ind< imagepoint_nbr ;
          imagepoint_ind++ ) {
       imagepoint= inp_imagepoint_arr[imagepoint_ind];
       camera_ind= imagepoint.camera_ind;
       feat_ind= imagepoint.feat_ind;
       x= imagepoint.x;
       y= imagepoint.y;
       imagepoint_arr[imagepoint_ind]= imagepoint;
    }

    threedpoint_arr[threedpoint_ind].xyz[0]= xyz[0];
    threedpoint_arr[threedpoint_ind].xyz[1]= xyz[1];
    threedpoint_arr[threedpoint_ind].xyz[2]= xyz[2];
    threedpoint_arr[threedpoint_ind].rgb[0]= rgb[0];
    threedpoint_arr[threedpoint_ind].rgb[1]= rgb[1];
    threedpoint_arr[threedpoint_ind].rgb[2]= rgb[2];
    threedpoint_arr[threedpoint_ind].imagepoint_arr= imagepoint_arr;
    threedpoint_arr[threedpoint_ind].imagepoint_nbr= imagepoint_nbr;
 }

 /*
 Free inp_threedpoint_arr
 */

 sfm_free_threedpoints(
  inp_threedpoint_arr,
  inp_threedpoint_nbr
 );

 fprintf(stdout,"Number of 3D points removed because 0 image point = %d\n",
  count0);
 fprintf(stdout,"Number of 3D points removed because 1 image point = %d\n",
  count0b);
 fprintf(stdout,"Number of 3D points removed because behind ref camera = %d\n",
  count1);
 fprintf(stdout,"Number of 3D points removed because separation angle too small = %d\n",
  count2);
 fprintf(stdout,"Number of 3D points removed because too few image points = %d\n",
  count3);

 *pthreedpoint_arr= threedpoint_arr;
 *pthreedpoint_nbr= threedpoint_nbr;

}
