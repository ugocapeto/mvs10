#include "header.h"

void mvs_add_imagepoint_to_threedpoint(
 int inp_camera_ind,
 int inp_feat_ind,
 double inp_x,
 double inp_y,
 int inp_pixel,
 int threedpoint_ind,
 int **pixel2threedpoint_arr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr
)

/*
Add image point to 3D point
unless already there
When I say "already there", I mean an image point that's on inp_camera_ind
Recall that in mvs, in a track, there can be more than one image point per camera
*/

{

 sfm_imagepoint_struct *imagepoint_arr;
 int imagepoint_nbr;
 int imagepoint_ind;
 sfm_imagepoint_struct imagepoint;
 int camera_ind;
 int feat_ind;
 double x;
 double y;
 int count;

 imagepoint_arr= threedpoint_arr[threedpoint_ind].imagepoint_arr;
 imagepoint_nbr= threedpoint_arr[threedpoint_ind].imagepoint_nbr;

 /*
 Check if there is already an image point on inp_camera_ind
 */

 /*
 We don't want to add another image point on the same camera
 */

 count= 0;
 for ( imagepoint_ind= 0 ;
       imagepoint_ind< imagepoint_nbr ;
       imagepoint_ind++ ) {
    imagepoint= imagepoint_arr[imagepoint_ind];
    camera_ind= imagepoint.camera_ind;
    feat_ind= imagepoint.feat_ind;
    x= imagepoint.x;
    y= imagepoint.y;
    if ( camera_ind == inp_camera_ind ) {
       count++;
    }
 }
 if ( count > 0 ) {
    if ( count > 1 ) {
       mvs10_error_handler("sfm_add_imagepoint_to_threedpoint");
    }

    /*
    Do not add to image points of 3D point
    */

    goto END;
 }

 /*
 Need to add
 (inp_camera_ind,inp_feat_ind,inp_x,inp_y)
 to the list of image points for 3D point
 */

 if ( imagepoint_nbr == 0 ) {
    imagepoint_arr= (sfm_imagepoint_struct *)calloc(imagepoint_nbr+1,sizeof(sfm_imagepoint_struct));
 }
 else {
    imagepoint_arr= (sfm_imagepoint_struct *)realloc(imagepoint_arr,(imagepoint_nbr+1)*sizeof(sfm_imagepoint_struct));
 }
 imagepoint_arr[imagepoint_nbr].camera_ind= inp_camera_ind;
 imagepoint_arr[imagepoint_nbr].feat_ind= inp_feat_ind;
 imagepoint_arr[imagepoint_nbr].x= inp_x;
 imagepoint_arr[imagepoint_nbr].y= inp_y;
 imagepoint_nbr++;
 threedpoint_arr[threedpoint_ind].imagepoint_arr= imagepoint_arr;
 threedpoint_arr[threedpoint_ind].imagepoint_nbr= imagepoint_nbr;

 /*
 Make sure image point references 3D point
 */

 if ( pixel2threedpoint_arr[inp_camera_ind][inp_pixel] != -1 ) {
    mvs10_error_handler("sfm_add_imagepoint_to_threedpoint");
 }
 pixel2threedpoint_arr[inp_camera_ind][inp_pixel]= threedpoint_ind;

 END:;

}
