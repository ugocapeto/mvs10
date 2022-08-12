#include "header.h"

void mvs_lbfgs_bundle_adjustment(
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr,
 int lbfgs_max_iterations
)

/*
Difference with sfm_lbfgs_bundle_adjustment()
is that here
the camera extrinsinc parameters are constrained
*/

{

 ba_measurement_struct *measurement_arr;
 int measurement_nbr;
 ba_camera_internal_struct *camera_internal_arr;
 double *p_camera_arr;
 double *p_point_arr;
 int *p_camera2_arr;
 int *p_point2_arr;
 double *p_grad_camera_arr;
 double *p_grad_point_arr;
 int p_camera_nbr;
 int p_point_nbr;
 double P[3*4];
 double K[3*3];
 double R[3*3];
 double t[3];
 double omega[3];
 int camera_matrix_ind;
 int threedpoint_ind;
 double xyz[3];
 int rgb[3];
 sfm_imagepoint_struct *imagepoint_arr;
 int imagepoint_nbr;
 int imagepoint_ind;
 int camera_ind;
 int feat_ind;
 double x;
 double y;
 int measurement_ind;
 double KR[3*3];
 double Kt[3];
 int i;
 int j;
 sfm_imagepoint_struct imagepoint;
 double R_check[3*3];

 p_camera_nbr= camera_matrix_nbr;

 /*
 Let's fill up camera_internal_arr
 */

 if ( p_camera_nbr > 0 )
  camera_internal_arr= (ba_camera_internal_struct *)calloc(p_camera_nbr,sizeof(ba_camera_internal_struct));

 for ( camera_matrix_ind= 0 ;
       camera_matrix_ind< camera_matrix_nbr ;
       camera_matrix_ind++ ) {

    /*
    Get the camera matrix
    */

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

    camera_internal_arr[camera_matrix_ind].fx  = K[0*3+0];
    camera_internal_arr[camera_matrix_ind].skew= K[0*3+1];
    camera_internal_arr[camera_matrix_ind].cx  = K[0*3+2];
    camera_internal_arr[camera_matrix_ind].fy  = K[1*3+1];
    camera_internal_arr[camera_matrix_ind].cy  = K[1*3+2];
    camera_internal_arr[camera_matrix_ind].k1  = 0;
    camera_internal_arr[camera_matrix_ind].k2  = 0;
    camera_internal_arr[camera_matrix_ind].p1  = 0;
    camera_internal_arr[camera_matrix_ind].p2  = 0;
 }

 /*
 Let's fill up p_camera_arr
 */

 if ( p_camera_nbr > 0 )
  p_camera_arr= (double *)calloc(p_camera_nbr,6*sizeof(double));

 for ( camera_matrix_ind= 0 ;
       camera_matrix_ind< camera_matrix_nbr ;
       camera_matrix_ind++ ) {

    /*
    Get the camera matrix
    */

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
    Get the rotation vector
    */

    ba_create_rodrigues_param_from_rotation_matrix(
     R,
     omega
    ); 

    /*
    Let's make sure we get the same R
    when creating the rotation matrix from the rotation vector
    */

    ba_create_rotation_matrix_rodrigues(
     omega,
     R_check
    );

    p_camera_arr[camera_matrix_ind*6+0]= omega[0];
    p_camera_arr[camera_matrix_ind*6+1]= omega[1];
    p_camera_arr[camera_matrix_ind*6+2]= omega[2];
    p_camera_arr[camera_matrix_ind*6+3+0]= t[0];
    p_camera_arr[camera_matrix_ind*6+3+1]= t[1];
    p_camera_arr[camera_matrix_ind*6+3+2]= t[2];
 }

 p_point_nbr= threedpoint_nbr;

 /*
 Let's fill up p_point_arr
 */

 if ( p_point_nbr > 0 )
  p_point_arr= (double *)calloc(p_point_nbr,3*sizeof(double));

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

    p_point_arr[threedpoint_ind*3+0]= xyz[0];
    p_point_arr[threedpoint_ind*3+1]= xyz[1];
    p_point_arr[threedpoint_ind*3+2]= xyz[2];
 }

 /*
 Let's fill up p_camera2_arr
 p_camera2_arr != 0 constrains the corresponding camera parameter
 */

 if ( p_camera_nbr > 0 )
  p_camera2_arr= (int *)calloc(p_camera_nbr,6*sizeof(int));

 /*
 Constrain all cameras
 */

 for ( camera_matrix_ind= 0 ;
       camera_matrix_ind< camera_matrix_nbr ;
       camera_matrix_ind++ ) {

    p_camera2_arr[camera_matrix_ind*6+0]= 1;
    p_camera2_arr[camera_matrix_ind*6+1]= 1;
    p_camera2_arr[camera_matrix_ind*6+2]= 1;
    p_camera2_arr[camera_matrix_ind*6+3+0]= 1;
    p_camera2_arr[camera_matrix_ind*6+3+1]= 1;
    p_camera2_arr[camera_matrix_ind*6+3+2]= 1;
 }

 /*
 Let's fill up p_point2_arr
 p_point2_arr != 0 constrains the corresponding point parameter
 */

 if ( p_point_nbr > 0 )
  p_point2_arr= (int *)calloc(p_point_nbr,3*sizeof(int));

 /*
 Let's fill measurement_arr
 */

 measurement_arr= 0;
 measurement_nbr= 0;

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

       sfm_get_camera_matrix_ind(
        camera_matrix_arr,
        camera_matrix_nbr,
        camera_ind,
        &camera_matrix_ind
       );

       if ( measurement_nbr == 0 ) {
          measurement_arr= (ba_measurement_struct *)calloc(measurement_nbr+1,sizeof(ba_measurement_struct));
       }
       else {
          measurement_arr= (ba_measurement_struct *)realloc(measurement_arr,(measurement_nbr+1)*sizeof(ba_measurement_struct));
       }

       measurement_ind= measurement_nbr;
       measurement_nbr++;

       /*
       Store
       camera/view index w/r to camera_matrix_arr
       3D point index
       image coordinates in homogeneous coordinates
       */

       measurement_arr[measurement_ind].camera_ind= camera_matrix_ind;
       measurement_arr[measurement_ind].point_ind= threedpoint_ind;
       measurement_arr[measurement_ind].x3[0]= x;
       measurement_arr[measurement_ind].x3[1]= y;
       measurement_arr[measurement_ind].x3[2]= 1;
    }
 }

 /*
 Allocate memory for p_grad_camera_arr
 Derivatives of function to minimize
 w/r to camera parameters
 */

 if ( p_camera_nbr > 0 )
  p_grad_camera_arr= (double *)calloc(p_camera_nbr,6*sizeof(double));

 /*
 Allocate memory for p_grad_point_arr
 Derivatives of function to minimize
 w/r to point parameters
 */

 if ( p_point_nbr > 0 )
  p_grad_point_arr= (double *)calloc(p_point_nbr,3*sizeof(double));

 ba_lbfgs_main(
  measurement_arr,
  measurement_nbr,
  camera_internal_arr,
  p_camera_arr,
  p_point_arr,
  p_camera2_arr,
  p_point2_arr,
  p_grad_camera_arr,
  p_grad_point_arr,
  p_camera_nbr,
  p_point_nbr,
  lbfgs_max_iterations
 );

 /*
 Update the camera matrices
 */

 for ( camera_matrix_ind= 0 ;
       camera_matrix_ind< camera_matrix_nbr ;
       camera_matrix_ind++ ) {

    /*
    Get the current camera matrix
    */

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
    Get new rotation vector and translation vector
    as found by the solver
    */

    omega[0]= p_camera_arr[camera_matrix_ind*6+0];
    omega[1]= p_camera_arr[camera_matrix_ind*6+1];
    omega[2]= p_camera_arr[camera_matrix_ind*6+2];
    t[0]= p_camera_arr[camera_matrix_ind*6+3+0];
    t[1]= p_camera_arr[camera_matrix_ind*6+3+1];
    t[2]= p_camera_arr[camera_matrix_ind*6+3+2];

    /*
    Compute rotation matrix from rotation vector
    */

    ba_create_rotation_matrix_rodrigues(
     omega,
     R
    );

    /*
    Compute P = K[R|t]
    */

    math_matrix_matrix_product(
     K,
     3,
     3,
     R,
     3,
     3,
     KR 
    );

    math_matrix_vector_product(
     K,
     3,
     3,
     t,
     3,
     Kt 
    );

    for ( i= 0 ; i< 3 ; i++ ) {
       for ( j= 0 ; j< 3 ; j++ ) {
          P[i*4+j]= KR[i*3+j];
       }
       j= 3;
       P[i*4+j]= Kt[i];
    }

    /*
    Update the camera matrix
    */

    sfm_update_camera_matrix(
     camera_matrix_arr,
     camera_matrix_nbr,
     camera_matrix_ind,
     camera_ind,
     P,
     K,
     R,
     t
    );
 }

 /*
 Update the points
 */

 for ( threedpoint_ind= 0 ;
       threedpoint_ind< threedpoint_nbr ;
       threedpoint_ind++ ) {

    /*
    Get the current point coordinates
    */

    xyz[0]= threedpoint_arr[threedpoint_ind].xyz[0];
    xyz[1]= threedpoint_arr[threedpoint_ind].xyz[1];
    xyz[2]= threedpoint_arr[threedpoint_ind].xyz[2];

    /*
    Get the new point coordinates
    as found by the solver
    */

    xyz[0]= p_point_arr[threedpoint_ind*3+0];
    xyz[1]= p_point_arr[threedpoint_ind*3+1];
    xyz[2]= p_point_arr[threedpoint_ind*3+2];

    /*
    Update the point coordinates
    */

    threedpoint_arr[threedpoint_ind].xyz[0]= xyz[0];
    threedpoint_arr[threedpoint_ind].xyz[1]= xyz[1];
    threedpoint_arr[threedpoint_ind].xyz[2]= xyz[2];
 }

 /*
 Free camera_internal_arr
 */

 if ( p_camera_nbr > 0 )
  free(camera_internal_arr);

 /*
 Free p_camera_arr
 */

 if ( p_camera_nbr > 0 )
  free(p_camera_arr);

 /*
 Free p_camera2_arr
 */

 if ( p_camera_nbr > 0 )
  free(p_camera2_arr);

 /*
 Free p_point_arr
 */

 if ( p_point_nbr > 0 )
  free(p_point_arr);

 /*
 Free p_point2_arr
 */

 if ( p_point_nbr > 0 )
  free(p_point2_arr);

 /*
 Free p_grad_camera_arr
 */

 if ( p_camera_nbr > 0 )
  free(p_grad_camera_arr);

 /*
 Free p_grad_point_arr
 */

 if ( p_point_nbr > 0 )
  free(p_grad_point_arr);

 /*
 Free measurement_arr
 */

 if ( measurement_nbr > 0 )
  free(measurement_arr);

}
