#include "header.h"

void mvs_get_camera_pair_stats_from_sfm(
 char **image_filename_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr,
 int camera1_ind,
 int camera2_ind,
 int *pmatch_nbr,
 double *pavg_angle,
 double *pdist
)

{

 int camera_matrix_ind1;
 int camera_matrix_ind2;
 double P1[3*4];
 double K1[3*3];
 double R1[3*3];
 double t1[3];
 double C1[3];
 double P2[3*4];
 double K2[3*3];
 double R2[3*3];
 double t2[3];
 double C2[3];
 int i;
 double vec[3];
 double norm2;
 double norm;
 double dist;
 int match_nbr;
 double avg_angle;
 int threedpoint_ind;
 double xyz[3];
 int rgb[3];
 sfm_imagepoint_struct *imagepoint_arr;
 int imagepoint_nbr;
 int count1;
 int count2;
 int imagepoint_ind;
 sfm_imagepoint_struct imagepoint;
 int camera_ind;
 int feat_ind;
 double x;
 double y;
 double vec1[3];
 double vec2[3];
 double dotp;
 double cosine;
 double angle_rad;
 double angle;
 double pi=acos(-1);

 /*
 Get camera matrix for camera 1
 */

 sfm_get_camera_matrix_ind(
  camera_matrix_arr,
  camera_matrix_nbr,
  camera1_ind,
  &camera_matrix_ind1
 );

 sfm_get_camera_matrix(
  camera_matrix_arr,
  camera_matrix_nbr,
  camera_matrix_ind1,
  &camera1_ind,
  P1,
  K1,
  R1,
  t1
 );

 /*
 Get camera center for camera 1
 */

 sfm_compute_C_from_R_t(
  R1,
  t1,
  C1
 );

 /*
 Get camera matrix for camera 2
 */

 sfm_get_camera_matrix_ind(
  camera_matrix_arr,
  camera_matrix_nbr,
  camera2_ind,
  &camera_matrix_ind2
 );

 sfm_get_camera_matrix(
  camera_matrix_arr,
  camera_matrix_nbr,
  camera_matrix_ind2,
  &camera2_ind,
  P2,
  K2,
  R2,
  t2
 );

 /*
 Get camera center for camera 2
 */

 sfm_compute_C_from_R_t(
  R2,
  t2,
  C2
 );

 /*
 Compute the distance between the camera centers
 aka the baseline
 */

 for ( i= 0 ; i< 3 ; i++ )
  vec[i]= C2[i]-C1[i];

 norm2= math_vector_vector_product(
  vec,
  3,
  vec,
  3
 );
 norm= sqrt(norm2);

 dist= norm;

 (*pdist)= dist;

 /*
 Initialize number of matches
 */

 match_nbr= 0;

 /*
 Initialize average separation angle
 */

 avg_angle= 0.0;

 /*
 Loop through the current 3D points
 */

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

    count1= 0;
    count2= 0;

    for ( imagepoint_ind= 0 ;
          imagepoint_ind< imagepoint_nbr ;
          imagepoint_ind++ ) {
       imagepoint= imagepoint_arr[imagepoint_ind];
       camera_ind= imagepoint.camera_ind;
       feat_ind= imagepoint.feat_ind;
       x= imagepoint.x;
       y= imagepoint.y;

       if ( camera_ind == camera1_ind )
        count1++;
       if ( camera_ind == camera2_ind )
        count2++;
    }

    if ( !(count1 == 1 && count2 == 1) )
     continue;

    match_nbr++;

    /*
    Compute the separation angle
    */

    /*
    Consider the normalized vector
    that goes from the 3D point to the camera center
    of camera 1
    */

    for ( i= 0 ; i< 3 ; i++ )
     vec[i]= C1[i]-xyz[i];

    norm2= math_vector_vector_product(
     vec,
     3,
     vec,
     3
    );
    norm= sqrt(norm2);

    for ( i= 0 ; i< 3 ; i++ )
     vec[i]/= norm;

    for ( i= 0 ; i< 3 ; i++ )
     vec1[i]= vec[i];

    /*
    Consider the normalized vector
    that goes from the 3D point to the camera center
    of camera 2
    */

    for ( i= 0 ; i< 3 ; i++ )
     vec[i]= C2[i]-xyz[i];

    norm2= math_vector_vector_product(
     vec,
     3,
     vec,
     3
    );
    norm= sqrt(norm2);

    for ( i= 0 ; i< 3 ; i++ )
     vec[i]/= norm;

    for ( i= 0 ; i< 3 ; i++ )
     vec2[i]= vec[i];

    dotp= math_vector_vector_product(
     vec1,
     3,
     vec2,
     3
    );

    cosine= dotp;

    angle_rad= acos(cosine);

    angle= angle_rad*180/pi;

    avg_angle+= angle;
 }

 if ( match_nbr > 0 )
  avg_angle/= (double)match_nbr;

 (*pmatch_nbr)= match_nbr;

 (*pavg_angle)= avg_angle;

}
