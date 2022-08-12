#include "header.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <time.h>
#include <iomanip>
#include <algorithm>
using namespace std;

void mvs_pba_bundle_adjustment(
 char **image_filename_arr,
 double *focal_length_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr
)

/*
See LoadNVM() in pba/src/pba/util.h
for how to store the input to the bundle adjuster
See main() in pba/src/driver/driver.cpp
for how to call the bundle adjuster
See SaveNVM() in pba/src/pba/util.h
for how to read the output from the bundle adjuster 
*/

{

 vector<CameraT>        camera_data;    //camera (input/ouput)
 vector<Point3D>        point_data;     //3D point(iput/output)
 vector<Point2D>        measurements;   //measurment/projection vector
 vector<int>            camidx, ptidx;  //index of camera/point for each projection
 //vector<string>         names;          //from NVM file, not used in bundle adjustment
 vector<int>            ptc;            //from NVM file, not used in bundle adjustment
 int ncam;
 double c[3];
 double d[2];
 int npoint;
 double pt[3];
 int cc[3];
 int npj;
 int cidx;
 int fidx;
 double imx;
 double imy;
 int nproj;
 int j;
 int je;

 int camera_ind;
 char image_filename[80];
 int err_flag;
 int *image_arr;
 int width;
 int height;
 double f;
 double K[3*3];
 double R[3*3];
 double C[3];
 double q[4];
 int threedpoint_ind;
 double xyz[3];
 int rgb[3];
 sfm_imagepoint_struct *imagepoint_arr;
 int imagepoint_nbr;
 double P[3*4];
 int i;
 int imagepoint_ind;
 sfm_imagepoint_struct imagepoint;
 int feat_ind;
 double x;
 double y;
 int camera_matrix_ind;
 double t[3];
 int width0;
 int height0;
 double KR[3*3];
 double Kt[3];

 ncam= camera_matrix_nbr;

 camera_data.resize(ncam); // allocate the camera data
 //names.resize(ncam);

 /*
 Assume all images have the same widht and height
 */

 width0= 0;
 height0= 0;

 /*
 For each camera, get and store:
 - focal length
 - quaternion
 - camera center 
 - distortion parameters
 */

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
    Get filename for image
    */

    strcpy(image_filename,image_filename_arr[camera_ind]);

    /*
    Store filename for image 
    */

    i= camera_matrix_ind;
    //names[i] = image_filename;

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

    if ( width0 == 0 ) {
       width0= width;
    }
    else if ( width != width0 ) {
       sfm10_error_handler("sfm_print_threedpoints_nvm");
    }

    if ( height0 == 0 ) {
       height0= height;
    }
    else if ( height != height0 ) {
       sfm10_error_handler("sfm_print_threedpoints_nvm");
    }

    /*
    Get the focal length
    */

    f= focal_length_arr[camera_ind];

    /*
    Store the focal length in camera_data
    */

    i= camera_matrix_ind;
    camera_data[i].SetFocalLength(f);

    /*
    Extract the quaternion from the rotation matrix
    */

    compute_quaternion_from_rotation(
     R,
     q
    );

    /*
    Store the quaternion in camera_data
    */

    i= camera_matrix_ind;
    camera_data[i].SetQuaternionRotation(q);        //quaternion from the file

    /*
    Get the camera center from R and t
    */

    sfm_compute_C_from_R_t(
     R,
     t,
     C
    );

    /*
    Store the camera center in camera_data
    */

    c[0]= C[0];
    c[1]= C[1];
    c[2]= C[2];
    i= camera_matrix_ind;
    camera_data[i].SetCameraCenterAfterRotation(c); //camera center from the file

    /*
    Store the distortion parameters in camera_data
    */

    d[0]= 0;
    d[1]= 0;
    i= camera_matrix_ind;
    camera_data[i].SetNormalizedMeasurementDistortion(d[0]);
 }

 npoint= threedpoint_nbr;

 point_data.resize(npoint);

 /*
 For each 3D point, get and store:
 XYZ, RGB, number of measurements, list of measurements
 Measurement = image index, feature index, xy
 */

 nproj= 0;

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

    if ( imagepoint_nbr == 0 ) {
       sfm10_error_handler("sfm_print_threedpoints_in");
    }

    pt[0]= xyz[0];
    pt[1]= xyz[1];
    pt[2]= xyz[2];

    cc[0]= rgb[0];
    cc[1]= rgb[1];
    cc[2]= rgb[2];

    npj= imagepoint_nbr;

    /*
    For each measurement, get and store:
    image index, feature index, xy
    Note that xy is expected to be w/r to image center
    */

    for ( imagepoint_ind= 0 ;
          imagepoint_ind< imagepoint_nbr ;
          imagepoint_ind++ ) {
       imagepoint= imagepoint_arr[imagepoint_ind];
       camera_ind= imagepoint.camera_ind;
       feat_ind= imagepoint.feat_ind;
       x= imagepoint.x;
       y= imagepoint.y;

       /*
       Get camera index w/t to camera_matrix_arr
       */ 

       sfm_get_camera_matrix_ind(
        camera_matrix_arr,
        camera_matrix_nbr,
        camera_ind,
        &camera_matrix_ind
       );

       /*
       We need the width and height of the image
       but it would be too expensive to load the image
       to get that info
       */

       width= width0; 
       height= height0;

       /*
       Store:
       - camera index
       - feature index
       - coordinates on image plane w/r to center of image
       */

       cidx= camera_matrix_ind;
       fidx= feat_ind;
       imx= x-(double)width/2.;
       imy= y-(double)height/2.;

       camidx.push_back(cidx);    //camera index
       i= threedpoint_ind;
       ptidx.push_back(i);        //point index

       //add a measurement to the vector
       measurements.push_back(Point2D(imx, imy));
       nproj ++;
    }

    /*
    Store:
    - point coordinates
    - rgb color
    */

    i= threedpoint_ind;
    point_data[i].SetPoint(pt);
    ptc.insert(ptc.end(), cc, cc + 3);
 }

 /*
 Run bundle adjustment
 */

    /////////////////////////////////////////////////////////////////////////////////////////
    ParallelBA::DeviceT device = ParallelBA::PBA_CUDA_DEVICE_DEFAULT;
    //if(strstr(driver_argument, "--float"))          device = ParallelBA::PBA_CPU_FLOAT;
    //else if(strstr(driver_argument, "--double"))    device = ParallelBA::PBA_CPU_DOUBLE;

    //Use doubles
    device = ParallelBA::PBA_CPU_DOUBLE;

    /////////////////////////////////////////////////////////////////////
    ParallelBA pba(device);          //You should reusing the same object for all new data

    //set verbose level to 0
    //pba.GetInternalConfig()->__verbose_level = 0;

    /////////////////////////////////////////////////////////
    //Parameters can be changed before every call of RunBundleAdjustment
    //But do not change them from another thread when it is running BA.
    //pba.ParseParam(argc, argv);      //indirect parameter tuning from commandline
    //pba.SetFixedIntrinsics(true); //if your focal lengths are calibrated.
    //           equivalent to pba.GetInternalConfig()->__fixed_focallength = true;
    //pba.EnableRadialDistortion(*); // if you want to enable radial distortion
    //           equivalent to pba.GetInternalConfig()->__use_radial_distortion = *;
    //check src/pba/ConfigBA.h for more details on the parameter system

    //Fix the focal lengths
    pba.SetFixedIntrinsics(true);

    ////////////////////////////////////
    //Tweaks before bundle adjustment
    //1. For each camera, you can call CameraT::SetConstantCamera() to keep it unchanged.
    //2. pba.SetNextBundleMode(ParallelBA::BUNDLE_ONLY_MOTION) //chose a truncated mode?

    //Fix all camera parameters
    for ( camera_matrix_ind= 0 ;
          camera_matrix_ind< camera_matrix_nbr ;
          camera_matrix_ind++ ) {
       camera_data[camera_matrix_ind].SetConstantCamera();
    }

    ////////////////////////////////////////////////////////////////
    pba.SetCameraData(camera_data.size(),  &camera_data[0]);                        //set camera parameters
    pba.SetPointData(point_data.size(), &point_data[0]);                            //set 3D point data
    pba.SetProjection(measurements.size(), &measurements[0], &ptidx[0], &camidx[0]);//set the projections
   
    //vector<int> cmask;
    //if(strstr(driver_argument, "--common"))
    //{
    //        cmask.resize(camera_data.size(), 0);
    //        pba.SetFocalMask(&cmask[0]);
    //}

    //WARNING: measumrents must be stored in correct order
    //all measutments (in different images) for a single 3D point must be stored continously,
    //Currently, there is no order verification internally.
    //Basically, ptidx should be non-decreasing

    //////////////////////////////////////////////////////
    //pba.SetTimeBudget(10);      //use at most 10 seconds?
    pba.RunBundleAdjustment();    //run bundle adjustment, and camera_data/point_data will be modified

    //Write the optimized system to file
    //const char*  outpath = pba.GetInternalConfig()->GetOutputParam();
    //SaveModelFile(outpath, camera_data, point_data, measurements, ptidx, camidx, photo_names, point_color);
    //It is easy to visualize the camera/points yourself,

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

    i= camera_matrix_ind;
    CameraT& cam = camera_data[i];

    for ( j= 0 ; j< 9 ; ++j )
     R[j]= cam.m[0][j];

    t[0]= cam.t[0];
    t[1]= cam.t[1];
    t[2]= cam.t[2];

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

 j= 0;

 for ( threedpoint_ind= 0 ;
       threedpoint_ind< threedpoint_nbr ;
       threedpoint_ind++ ) {

    /*
    Get the current point coordinates and color
    */

    xyz[0]= threedpoint_arr[threedpoint_ind].xyz[0];
    xyz[1]= threedpoint_arr[threedpoint_ind].xyz[1];
    xyz[2]= threedpoint_arr[threedpoint_ind].xyz[2];
    rgb[0]= threedpoint_arr[threedpoint_ind].rgb[0];
    rgb[1]= threedpoint_arr[threedpoint_ind].rgb[1];
    rgb[2]= threedpoint_arr[threedpoint_ind].rgb[2];
    imagepoint_arr= threedpoint_arr[threedpoint_ind].imagepoint_arr;
    imagepoint_nbr= threedpoint_arr[threedpoint_ind].imagepoint_nbr;

    i= threedpoint_ind;
    Point3D& pt = point_data[i];
    int * pc = &ptc[i * 3];

    /*
    Update point coordinates and color
    */

    threedpoint_arr[threedpoint_ind].xyz[0]= pt.xyz[0];
    threedpoint_arr[threedpoint_ind].xyz[1]= pt.xyz[1];
    threedpoint_arr[threedpoint_ind].xyz[2]= pt.xyz[2];
    threedpoint_arr[threedpoint_ind].rgb[0]= pc[0];
    threedpoint_arr[threedpoint_ind].rgb[1]= pc[1];
    threedpoint_arr[threedpoint_ind].rgb[2]= pc[2];

    /*
    Update the measurements
    */

    je = j;
    while(je < ptidx.size() && ptidx[je] == (int) i) je++;

    if ( (je-j) != imagepoint_nbr ) {
       sfm10_error_handler("mvs_pba_bundle_adjustment");
    }

    imagepoint_ind= 0;
    for(; j < je; ++j) {
       imagepoint= imagepoint_arr[imagepoint_ind];
       camera_ind= imagepoint.camera_ind;
       feat_ind= imagepoint.feat_ind;
       x= imagepoint.x;
       y= imagepoint.y;

       /*
       Get camera index w/t to camera_matrix_arr
       */

       sfm_get_camera_matrix_ind(
        camera_matrix_arr,
        camera_matrix_nbr,
        camera_ind,
        &camera_matrix_ind
       );

       /*
       We need the width and height of the image
       but it would be too expensive to load the image
       to get that info
       */

       width= width0;
       height= height0;

       if ( camidx[j] != camera_matrix_ind ) {
          sfm10_error_handler("mvs_pba_bundle_adjustment");
       }

       imagepoint.x= measurements[j].x+(double)width/2.;
       imagepoint.y= measurements[j].y+(double)height/2.;
       imagepoint_arr[imagepoint_ind]= imagepoint;

       imagepoint_ind++;
    }
 }

}
