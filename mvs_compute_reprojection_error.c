#include "header.h"

double mvs_compute_reprojection_error(
 char **image_filename_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 int camera_ind,
 double x,
 double y,
 double xyz[3]
)

{

 int camera_matrix_ind;
 double P[3*4];
 double K[3*3];
 double R[3*3];
 double t[3];
 double x3[3];
 double x_proj;
 double y_proj;
 double dist2;
 double dist;

 /*
 Get camera matrix for camera
 */

 sfm_get_camera_matrix_ind(
  camera_matrix_arr,
  camera_matrix_nbr,
  camera_ind,
  &camera_matrix_ind
 );

 sfm_get_camera_matrix(
  camera_matrix_arr,
  camera_matrix_nbr,
  camera_matrix_ind,
  &camera_ind,
  P,
  K,
  R,
  t
 );

 /*
 Compute the projection of the 3D point onto the image
 */

 ba_project_point(
  K,
  R,
  t,
  xyz,
  x3
 );

 x_proj= x3[0]/x3[2];
 y_proj= x3[1]/x3[2];

 dist2= (x_proj-x)*(x_proj-x)+
           (y_proj-y)*(y_proj-y);
 dist= sqrt(dist2);

 return dist;

}
