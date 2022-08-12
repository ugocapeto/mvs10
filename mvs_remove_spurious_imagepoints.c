#include "header.h"

void mvs_remove_spurious_imagepoints(
 char **image_filename_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct **pthreedpoint_arr,
 int *pthreedpoint_nbr,
 double max_reproj_error
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
 int imagepoint_ind;
 sfm_imagepoint_struct imagepoint;
 double x;
 double y;
 int inp_imagepoint_nbr;
 int inp_imagepoint_ind;
 double reproj_error;
 int inp_count;
 int count;

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

 inp_count= 0;
 count= 0;

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
    inp_imagepoint_nbr= inp_threedpoint_arr[inp_threedpoint_ind].imagepoint_nbr;

    /*
    Check for 3D point that doesn't actually exist
    */

    if ( inp_imagepoint_nbr == 0 ) {
       if ( inp_imagepoint_arr != 0 ) {
          mvs10_error_handler("mvs_remove_spurious_imagepoints");
       }
       continue;
    }

    /*
    Let's rebuild the image points
    discarding any with a large reprojection error
    */

    imagepoint_arr= 0;
    imagepoint_nbr= 0;

    for ( inp_imagepoint_ind= 0 ;
          inp_imagepoint_ind< inp_imagepoint_nbr ;
          inp_imagepoint_ind++ ) {
       imagepoint= inp_imagepoint_arr[inp_imagepoint_ind];
       camera_ind= imagepoint.camera_ind;
       feat_ind= imagepoint.feat_ind;
       x= imagepoint.x;
       y= imagepoint.y;

       inp_count++;

       /*
       Compute the reprojection error
       */

       reproj_error= mvs_compute_reprojection_error(
        image_filename_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        camera_ind,
        x,
        y,
        xyz
       );

       if ( reproj_error > max_reproj_error )
        continue;

       /*
       Add the image point
       */

       if ( imagepoint_nbr == 0 ) {
          imagepoint_arr= (sfm_imagepoint_struct *)calloc(imagepoint_nbr+1,sizeof(sfm_imagepoint_struct));
       }
       else {
          imagepoint_arr= (sfm_imagepoint_struct *)realloc(imagepoint_arr,(imagepoint_nbr+1)*sizeof(sfm_imagepoint_struct));
       }
       imagepoint_ind= imagepoint_nbr;
       imagepoint_nbr++;
       imagepoint_arr[imagepoint_ind]= imagepoint;

       count++;
    }

    /*
    If the 3D point has no image point that remains,
    it is discarded
    */

    if ( imagepoint_nbr == 0 )
     continue;

    /*
    If the 3D point has only 1 image point that remains,
    it is discarded as well
    */

    if ( imagepoint_nbr == 1 ) {
       if ( imagepoint_nbr > 0 )
        free(imagepoint_arr);
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

    threedpoint_arr[threedpoint_ind].xyz[0]= xyz[0];
    threedpoint_arr[threedpoint_ind].xyz[1]= xyz[1];
    threedpoint_arr[threedpoint_ind].xyz[2]= xyz[2];
    threedpoint_arr[threedpoint_ind].rgb[0]= rgb[0];
    threedpoint_arr[threedpoint_ind].rgb[1]= rgb[1];
    threedpoint_arr[threedpoint_ind].rgb[2]= rgb[2];
    threedpoint_arr[threedpoint_ind].imagepoint_arr= imagepoint_arr;
    threedpoint_arr[threedpoint_ind].imagepoint_nbr= imagepoint_nbr;
 }

 fprintf(stdout,"Number of image points (before) = %d\n",
  inp_count);
 fprintf(stdout,"Number of image points (after) = %d\n",
  count);

 /*
 Free inp_threedpoint_arr
 */

 sfm_free_threedpoints(
  inp_threedpoint_arr,
  inp_threedpoint_nbr
 );

 *pthreedpoint_arr= threedpoint_arr;
 *pthreedpoint_nbr= threedpoint_nbr;

}
