#include "header.h"

void mvs_read_nvm(
 char *filename,
 char ***pimage_filename_arr,
 int **pwidth_arr,
 int **pheight_arr,
 double **pfocal_length_arr,
 int *pcamera_nbr,
 sfm_camera_matrix_struct **pcamera_matrix_arr,
 int *pcamera_matrix_nbr,
 sfm_threedpoint_struct **pthreedpoint_arr,
 int *pthreedpoint_nbr
)

{

 char **image_filename_arr;
 double *focal_length_arr;
 int camera_nbr;
 sfm_camera_matrix_struct *camera_matrix_arr;
 int camera_matrix_nbr;
 sfm_threedpoint_struct *threedpoint_arr;
 int threedpoint_nbr;
 FILE *fp;
 char line[256];
 int camera_matrix_ind;
 char image_filename[80];
 int err_flag;
 int *image_arr;
 int width;
 int height;
 double focal_length;
 int i;
 double q[4];
 double C[3];
 double d[2];
 double R[3*3];
 double t[3];
 double K[3*3];
 double P[3*4];
 int camera_ind;
 double xyz[3];
 int rgb[3];
 int imagepoint_nbr;
 sfm_imagepoint_struct *imagepoint_arr;
 int threedpoint_ind;
 int imagepoint_ind;
 double xy[2];
 sfm_imagepoint_struct imagepoint;
 int feat_ind;
 double x;
 double y;
 int *width_arr;
 int *height_arr;

 image_filename_arr= 0;
 width_arr= 0;
 height_arr= 0;
 focal_length_arr= 0;
 camera_nbr= 0;
 camera_matrix_arr= 0;
 camera_matrix_nbr= 0;
 threedpoint_arr= 0;
 threedpoint_nbr= 0;

 fp= fopen(filename,"r");

 /*
 Read file version header
 */

 fscanf(fp,"%s",line);
 if ( strcmp(line,"NVM_V3") != 0 ) {
    mvs10_error_handler("mvs_read_nvm");
 }

 /*
 Read number of cameras
 */

 fscanf(fp,"%d",&camera_nbr);

 if ( camera_nbr > 0 )
  image_filename_arr= (char **)calloc(camera_nbr,sizeof(char *));
 if ( camera_nbr > 0 )
  width_arr= (int *)calloc(camera_nbr,sizeof(int));
 if ( camera_nbr > 0 )
  height_arr= (int *)calloc(camera_nbr,sizeof(int));
 if ( camera_nbr > 0 )
  focal_length_arr= (double *)calloc(camera_nbr,sizeof(double));

 /*
 For each camera, read:
 File name, focal length, quaternion WXYZ, camera center, radial distortion, 0
 */

 for ( camera_ind= 0 ;
       camera_ind< camera_nbr ;
       camera_ind++ ) {

    /*
    Get filename for the image
    */

    fscanf(fp,"%s",image_filename);

    /*
    Store the filename for the image
    in image_filename_arr
    */

    image_filename_arr[camera_ind]= (char *)calloc(80,sizeof(char));
    strcpy(image_filename_arr[camera_ind],image_filename);

    /*
    Load image
    */

    err_flag= load_rgb_image(
     image_filename,
     &image_arr,
     &width,
     &height
    );

    free(image_arr);

    /*
    Store the width in width_arr
    */

    width_arr[camera_ind]= width;

    /*
    Store the height in height_arr
    */

    height_arr[camera_ind]= height;

    /*
    Get focal length (in pixel units)
    */

    fscanf(fp,"%lg",&focal_length);

    /*
    Store the focal length (in pixel units)
    in focal_length_arr
    */

    focal_length_arr[camera_ind]= focal_length;

    /*
    Get rotation parameters (in quaternion form)
    */

    for ( i= 0 ; i< 4 ; i++ )
     fscanf(fp,"%lg",&q[i]);

    /*
    Get camera center
    */

    for ( i= 0; i< 3 ; i++ )
     fscanf(fp,"%lg",&C[i]);

    /*
    Get radial distortion
    */

    for ( i= 0; i< 2 ; i++ )
     fscanf(fp,"%lg",&d[i]);

    /*
    From the quaternion,
    we need to extract the camera rotation
    */

    compute_rotation_from_quaternion(
     q,
     R
    );

    /*
    Compute the translation
    t=-R*C
    */

    mvs_compute_t_from_R_C(
     R,
     C,
     t
    );

    /*
    Set the calibration matrix
    */

    /*
    It is assumed that the origin of the image coordinate system
    is at top-left with y axis going down
    It is also assumed that the y axis of the camera coordinate system
    points downward like in VisualSFM
    */

    K[0*3+0]= focal_length;
    K[0*3+1]= 0;
    K[0*3+2]= (double)width/2;

    K[1*3+0]= 0;
    K[1*3+1]= focal_length;
    K[1*3+2]= (double)height/2;

    K[2*3+0]= 0;
    K[2*3+1]= 0;
    K[2*3+2]= 1;

    /*
    Compute P=K[R|t]
    */

    sfm_compute_P_from_K_R_t(
     K,
     R,
     t,
     P
    );

    /*
    Add P to camera_matrix_arr
    */

    sfm_add_camera_matrix(
     camera_ind,
     P,
     K,
     R,
     t,
     &camera_matrix_arr,
     &camera_matrix_nbr,
     &camera_matrix_ind
    );

    if ( camera_matrix_ind != camera_ind ) {
       mvs10_error_handler("mvs_read_nvm");
    }
 }

 /*
 Get number of 3D points
 */

 fscanf(fp,"%d",&threedpoint_nbr);

 if ( threedpoint_nbr > 0 ) {
    threedpoint_arr= (sfm_threedpoint_struct *)calloc(threedpoint_nbr,sizeof(sfm_threedpoint_struct));
 }

 /*
 For each 3D point, get:
 XYZ, RGB, number of measurements, list of measurements
 Measurement = image index, feature index, xy
 */

 for ( threedpoint_ind= 0 ;
       threedpoint_ind< threedpoint_nbr ;
       threedpoint_ind++ ) {

    /*
    Get point location
    */

    for ( i= 0 ; i< 3 ; i++ )
     fscanf(fp,"%lg",&xyz[i]);

    /*
    Get rgb color
    */

    for ( i= 0 ; i< 3 ; i++ )
     fscanf(fp,"%d",&rgb[i]);

    /*
    Get number of measurements
    */

    fscanf(fp,"%d",&imagepoint_nbr);

    threedpoint_arr[threedpoint_ind].xyz[0]= xyz[0];
    threedpoint_arr[threedpoint_ind].xyz[1]= xyz[1];
    threedpoint_arr[threedpoint_ind].xyz[2]= xyz[2];
    threedpoint_arr[threedpoint_ind].rgb[0]= rgb[0];
    threedpoint_arr[threedpoint_ind].rgb[1]= rgb[1];
    threedpoint_arr[threedpoint_ind].rgb[2]= rgb[2];
    threedpoint_arr[threedpoint_ind].imagepoint_nbr= imagepoint_nbr;

    imagepoint_arr= 0;
    if ( imagepoint_nbr > 0 ) {
       imagepoint_arr= (sfm_imagepoint_struct *)calloc(imagepoint_nbr,sizeof(sfm_imagepoint_struct));
    }
    threedpoint_arr[threedpoint_ind].imagepoint_arr= imagepoint_arr;

    /*
    For each measurement, get:
    image index, feature index, xy
    Note that xy is expected to be w/r to image center
    */

    for ( imagepoint_ind= 0 ;
          imagepoint_ind< imagepoint_nbr ;
          imagepoint_ind++ ) {

       /*
       Get camera index
       */

       fscanf(fp,"%d",&camera_ind);

       /*
       Get feature index
       */

       fscanf(fp,"%d",&feat_ind);

       /*
       Get feature location
       */

       for ( i= 0 ; i< 2 ; i++ )
        fscanf(fp,"%lg",&xy[i]);

       x= xy[0];
       y= xy[1];

       width= width_arr[camera_ind];
       height= height_arr[camera_ind];

       imagepoint.camera_ind= camera_ind;
       imagepoint.feat_ind= feat_ind;
       imagepoint.x= x+(double)width/2;
       imagepoint.y= y+(double)height/2;
       imagepoint_arr[imagepoint_ind]= imagepoint;
    }
 }

 fclose(fp);

 (*pimage_filename_arr)= image_filename_arr;
 (*pwidth_arr)= width_arr;
 (*pheight_arr)= height_arr;
 (*pfocal_length_arr)= focal_length_arr;
 (*pcamera_nbr)= camera_nbr;
 (*pcamera_matrix_arr)= camera_matrix_arr;
 (*pcamera_matrix_nbr)= camera_matrix_nbr;
 (*pthreedpoint_arr)= threedpoint_arr;
 (*pthreedpoint_nbr)= threedpoint_nbr;

}
