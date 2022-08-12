#include "header.h"

int mvs_get_sfm_camera_ind(
 char **sfm_image_filename_arr,
 int camera_nbr,
 char *image_filename
)

{

 int camera_ind;

 for ( camera_ind= 0 ;
       camera_ind< camera_nbr ;
       camera_ind++ ) {
    if ( strcmp(sfm_image_filename_arr[camera_ind],image_filename) == 0 )
     break;
 }

 /*
 if ( !(camera_ind< camera_nbr) ) {
    mvs10_error_handler("mvs_get_sfm_camera_ind");
 }
 */

 return camera_ind;

}
