#include "header.h"

void mvs_check_threedpoints(
 char **image_filename_arr,
 int *width_arr,
 int *height_arr,
 double *focal_length_arr,
 int camera_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr
)

{

 int **pixel2threedpoint_arr;
 int camera_ind;
 int width;
 int height;
 int i;
 int j;
 int threedpoint_ind;
 double xyz[3];
 int rgb[3];
 sfm_imagepoint_struct *imagepoint_arr;
 int imagepoint_nbr;
 int imagepoint_ind;
 sfm_imagepoint_struct imagepoint;
 int feat_ind;
 double x;
 double y;
 int pixel;

 /*
 Initialize pixel2threedpoint_arr
 pixel2threedpoint_arr[camera_ind][pixel]= 3D point
 */

 pixel2threedpoint_arr= (int **)calloc(camera_nbr,sizeof(int *));
 for ( camera_ind= 0 ;
       camera_ind< camera_nbr ;
       camera_ind++ ) {
    width= width_arr[camera_ind];
    height= height_arr[camera_ind];
    pixel2threedpoint_arr[camera_ind]= (int *)calloc(width*height,sizeof(int));

    /*
    Initialize to no threedpoint for all pixels
    */

    for ( i= 0 ; i< height ; i++ ) {
       for ( j= 0 ; j< width ; j++ ) {
          pixel2threedpoint_arr[camera_ind][i*width+j]=-1;
       }
    }
 }
    
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

    for ( imagepoint_ind= 0 ;
          imagepoint_ind< imagepoint_nbr ;
          imagepoint_ind++ ) {
       imagepoint= imagepoint_arr[imagepoint_ind];
       camera_ind= imagepoint.camera_ind;
       feat_ind= imagepoint.feat_ind;
       x= imagepoint.x;
       y= imagepoint.y;

       /*
       Get width and height of image
       */

       width= width_arr[camera_ind];
       height= height_arr[camera_ind];

       /*
       Get closest pixel
       */

       j= (int)(x+0.5);
       if ( j < 0 ) j= 0;
       if ( j > width-1 ) j= width-1;

       i= (int)(y+0.5);
       if ( i < 0 ) i= 0;
       if ( i > height-1 ) i= height-1;

       pixel= i*width+j;

       /*
       Associate (camera_ind,pixel) with 3D point
       */

       if ( pixel2threedpoint_arr[camera_ind][pixel] != -1 ) {
          mvs10_error_handler("mvs_check_threedpoints");
       }
       pixel2threedpoint_arr[camera_ind][pixel]= threedpoint_ind;
    }
 }

 /*
 Free pixel2threedpoint_arr
 */

 for ( camera_ind = 0 ;
       camera_ind< camera_nbr ;
       camera_ind++ ) {
    free(pixel2threedpoint_arr[camera_ind]);
 }
 if ( camera_nbr > 0 )
  free(pixel2threedpoint_arr);

}
