#include "header.h"

int mvs_get_threedpoints_nbr(
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr
)

{

 int threedpoint_ind;
 double xyz[3];
 int rgb[3];
 sfm_imagepoint_struct *imagepoint_arr;
 int imagepoint_nbr;
 int count;

 count= 0;

 for ( threedpoint_ind= 0 ;
       threedpoint_ind< threedpoint_nbr ;
       threedpoint_ind++ ) {

    xyz[0]= threedpoint_arr[threedpoint_ind].xyz[0];
    xyz[1]= threedpoint_arr[threedpoint_ind].xyz[1];
    xyz[2]= threedpoint_arr[threedpoint_ind].xyz[2];
    rgb[0]= threedpoint_arr[threedpoint_ind].rgb[0];
    rgb[1]= threedpoint_arr[threedpoint_ind].rgb[1];
    rgb[2]= threedpoint_arr[threedpoint_ind].rgb[2];
    imagepoint_arr= threedpoint_arr[threedpoint_ind].imagepoint_arr;
    imagepoint_nbr= threedpoint_arr[threedpoint_ind].imagepoint_nbr;

    /*
    See if 3D point actually exists
    */

    if ( imagepoint_nbr == 0 ) { 
       if ( imagepoint_arr != 0 ) {
          mvs10_error_handler("sfm_get_threedpoints_nbr");
       }
       continue;
    }

    count++;
 }

 return count;

}
