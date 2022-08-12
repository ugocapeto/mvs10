#include "header.h"

void mvs_get_camera_pair_stats_from_match_file(
 char **image_filename_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr,
 int camera1_ind,
 int camera2_ind,
 char *filename_match,
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
 double P2[3*4];
 double K2[3*3];
 double R2[3*3];
 double t2[3];
 int match_nbr;
 double avg_angle;
 match_struct *match_arr;
 double C1[3];
 double C2[3];
 int i;
 double vec[3];
 double norm2;
 double norm;
 double dist;

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
 Read the matches
 that were used for rectification of the image pair
 */

 sfm_read_matches(
  filename_match,
  &match_arr,
  &match_nbr
 );

 /*
 Compute the average separation angle
 */

 avg_angle= sfm_compute_average_separation_angle(
  P1,
  K1,
  R1,
  t1,
  P2,
  K2,
  R2,
  t2,
  match_arr,
  match_nbr
 );

 /*
 Free the matches
 */

 if ( match_nbr > 0 )
  free(match_arr);

 (*pmatch_nbr)= match_nbr;

 (*pavg_angle)= avg_angle;

}
