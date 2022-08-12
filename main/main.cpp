#include "header.h"
#include "proto.h"

int main(
 int argc,
 char* argv[]
)

{

 FILE *fp;
 char filename[256];
 char filename_nvm[256];
 char **image_filename_arr;
 double *focal_length_arr;
 int camera_nbr;
 sfm_camera_matrix_struct *camera_matrix_arr;
 int camera_matrix_nbr;
 sfm_threedpoint_struct *threedpoint_arr;
 int threedpoint_nbr;
 int *width_arr;
 int *height_arr;
 int **pixel2threedpoint_arr;
 int camera1_ind;
 char filename1[80];
 int camera2_ind;
 char filename2[80];
 int width;
 int height;
 int width1;
 int height1;
 int width2;
 int height2;
 int radius;
 double alpha;
 double max_cost_color;
 double max_cost_gradient;
 int epsilon_int;
 int disp_tol;
 int *disp1_arr;
 int *disp2_arr;
 int *occ1_arr;
 int *occ2_arr;
 int i;
 int j;
 double H1[3*3];
 double H2[3*3];
 int camera_ind;
 int sampling_step;
 double min_separation_angle;
 double max_reproj_error;
 int min_d;
 int max_d;
 int *normalized_disp1_arr;
 int min_imagepoint_nbr;
 int ds_factor;
 int match_nbr;
 double avg_angle;
 double dist;
 double min_avg_separation_angle;
 int radius_gif;
 double max_theta_deg;
 int err_flag;
 char **sfm_image_filename_arr;
 double focal_length;
 char image_filename[80];
 int min_match_nbr;
 int sfm_camera_nbr;
 char filename_match[80];
 int sfm_camera1_ind;
 int sfm_camera2_ind;
 match_struct *match_arr;
 int min_imagepoint_nbr2;
 int nbr;
 double max_mean_error;

 /*
 Let's read in the mvs10 input file
 */

 fp= fopen("mvs10_input.txt","r");
 if ( fp == 0 ) {
    fprintf(stdout,"mvs10_input.txt not found!\n");
    return 1;
 }

 /*
 Get filename for nvm file
 */

 fscanf(fp,"%s",filename);

 fprintf(stdout,"nvm file = %s\n",filename);

 strcpy(filename_nvm,filename);

 /*
 Get the minimum number of matches (camera pair selection)
 */

 fscanf(fp,"%d",&min_match_nbr);

 fprintf(stdout,"Min match number (camera pair selection) = %d\n",
  min_match_nbr);

 /*
 Get the max mean vertical disparity error allowed
 */

 fscanf(fp,"%lg",&max_mean_error);

 fprintf(stdout,"Max mean vertical disparity error (camera pair selection) = %g\n",
  max_mean_error);

 /*
 Get the minimum average separation angle (camera pair selection)
 */

 fscanf(fp,"%lg",&min_avg_separation_angle);

 fprintf(stdout,"Min average separation angle (camera pair selection) = %g\n",
  min_avg_separation_angle);

 /*
 Get radius used to smooth the cost
 */

 fscanf(fp,"%d",&radius);

 fprintf(stdout,"radius (disparity map) = %d\n",radius);

 /*
 Get alpha
 */

 fscanf(fp,"%lg",&alpha);

 fprintf(stdout,"alpha (disparity map) = %g\n",alpha);

 /*
 Get truncation value for color cost
 */

 fscanf(fp,"%lg",&max_cost_color);

 fprintf(stdout,"color truncation (disparity map) = %g\n",max_cost_color);

 /*
 Get truncation value for gradient cost
 */

 fscanf(fp,"%lg",&max_cost_gradient);

 fprintf(stdout,"gradient truncation (disparity map) = %g\n",max_cost_gradient);

 /*
 Get epsilon
 */

 fscanf(fp,"%d",&epsilon_int);

 fprintf(stdout,"epsilon = 255^2*10^-%d (disparity map)\n",epsilon_int);

 /*
 Get the disparity tolerance used to detect occlusions
 */

 fscanf(fp,"%d",&disp_tol);

 fprintf(stdout,"disparity tolerance (disparity map)= %d\n",disp_tol);

 /*
 Get the downsampling factor
 */

 fscanf(fp,"%d",&ds_factor);

 fprintf(stdout,"downsampling factor (disparity map)= %d\n",ds_factor);

 /*
 Get the sampling step used to control the density of the reconstruction
 */

 fscanf(fp,"%d",&sampling_step);

 fprintf(stdout,"sampling step (dense reconstruction)= %d\n",sampling_step);

 /*
 Get the minimum separation angle (removal of low-confidence 3D points)
 */

 fscanf(fp,"%lg",&min_separation_angle);

 fprintf(stdout,"Min separation angle (low-confidence 3D points) = %g\n",
  min_separation_angle);

 /*
 Get the maximum reprojection error (removal of low-confidence image points)
 */

 fscanf(fp,"%lg",&max_reproj_error);

 fprintf(stdout,"Max reprojection error (low-confidence image points) = %g\n",
  max_reproj_error);

 /*
 Get the minimum number of image points per 3D point
 */

 fscanf(fp,"%d",&min_imagepoint_nbr);

 fprintf(stdout,"Min image point number (low-confidence 3D points) = %d\n",
  min_imagepoint_nbr);

 /*
 Get the radius for the animated gif frames
 */

 fscanf(fp,"%d",&radius_gif);

 fprintf(stdout,"Radius (animated gif frames) = %d\n",
  radius_gif);

 /*
 Get the angle amplitude for the animated gif frames
 */

 fscanf(fp,"%lg",&max_theta_deg);

 fprintf(stdout,"Angle amplitude (animated gif frames) = %g\n",
  max_theta_deg);

 /*
 Done reading the mvs10 input file
 */

 fclose(fp);

 /*
 Let's read in the sfm10 input file
 This is just to get the camera/view ordering that
 was used when sfm10 was run
 */

 fp= fopen("sfm10_input.txt","r");
 if ( fp == 0 ) {
    fprintf(stdout,"sfm10_input.txt not found!\n");
    return 1;
 }

 /*
 Get number of images/cameras
 */

 fscanf(fp,"%d",&sfm_camera_nbr);

 fprintf(stdout,"Number of cameras (sfm) = %d\n",
  sfm_camera_nbr);

 if ( sfm_camera_nbr > 0 )
  sfm_image_filename_arr= (char **)calloc(sfm_camera_nbr,sizeof(char *));

 /*
 Get the image filenames
 */

 for ( camera_ind= 0 ;
       camera_ind< sfm_camera_nbr ;
       camera_ind++ ) {
    fscanf(fp,"%s",image_filename);

    fprintf(stdout,"Image name (sfm) for camera %d = %s\n",
     camera_ind,image_filename);

    sfm_image_filename_arr[camera_ind]= (char *)calloc(80,sizeof(char));
    strcpy(sfm_image_filename_arr[camera_ind],image_filename);
 }

 /*
 Done with the sfm10 input file
 */

 fclose(fp);

 /*
 Read the nvm file
 */

 fprintf(stdout,"Reading the nvm file ...\n");

 mvs_read_nvm(
  filename_nvm,
  &image_filename_arr,
  &width_arr,
  &height_arr,
  &focal_length_arr,
  &camera_nbr,
  &camera_matrix_arr,
  &camera_matrix_nbr,
  &threedpoint_arr,
  &threedpoint_nbr
 );

 fprintf(stdout,"Reading the nvm file ... done.\n");

 /*
 Print the actual number of 3D points
 */

 nbr= mvs_get_threedpoints_nbr(
  threedpoint_arr,
  threedpoint_nbr
 );

 fprintf(stdout,"Number of 3D points = %d\n",
  nbr);

 /*
 Let's see if phase 1 has already been completed
 by checking if the file named "phase1.mvs" exists
 */

 fp= fopen("phase1.mvs","r");
 if ( fp != 0 ) {
    fclose(fp);
    goto PHASE2;
 }

 PHASE1:

 fprintf(stdout,"Rectifying the image pairs and computing the depth maps ...\n");

 for ( camera1_ind= 0 ;
       camera1_ind< camera_nbr ;
       camera1_ind++ ) {

    /*
    Get filename for image
    */

    strcpy(filename1,image_filename_arr[camera1_ind]);

    width1= width_arr[camera1_ind];
    height1= height_arr[camera1_ind];

    for ( camera2_ind= camera1_ind+1 ;
          camera2_ind< camera_nbr ;
          camera2_ind++ ) {

       fprintf(stdout,"Processing image pair %d and %d ...\n",
        camera1_ind,camera2_ind);

       /*
       Get filename for image
       */

       strcpy(filename2,image_filename_arr[camera2_ind]);

       width2= width_arr[camera2_ind];
       height2= height_arr[camera2_ind];

       if ( width2 != width1 ) {
          mvs10_error_handler("main");
       }
       if ( height2 != height1 ) {
          mvs10_error_handler("main");
       }

       width= width1;
       height= height1;

       /*
       Get some stats about this camera pair
       from Structure from Motion  (what is in nvm file)
       match_nbr = number of matches
       avg_angle = average separation angle
       dist = distance between camera centers (baseline)
       */

       mvs_get_camera_pair_stats_from_sfm(
        image_filename_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        threedpoint_arr,
        threedpoint_nbr,
        camera1_ind,
        camera2_ind,
        &match_nbr,
        &avg_angle,
        &dist
       );

       fprintf(stdout,"Number of matches (from nvm file) = %d\n",
        match_nbr);

       fprintf(stdout,"Average separation angle (from nvm file) = %g\n",
        avg_angle);

       fprintf(stdout,"Distance between the camera centers (from nvm file) = %g\n",
        dist);

       /*
       Let's get the name of the file
       where the matches have been stored for this camera pair
       */

       /*
       Get sfm camera index corresponding to filename1
       */

       sfm_camera1_ind= mvs_get_sfm_camera_ind(
        sfm_image_filename_arr,
        sfm_camera_nbr,
        filename1
       );

       if ( !(sfm_camera1_ind < sfm_camera_nbr) ) {
          mvs10_error_handler("mvs_rectify_image_pair");
       }

       /*
       Get sfm camera index corresponding to filename2
       */

       sfm_camera2_ind= mvs_get_sfm_camera_ind(
        sfm_image_filename_arr,
        sfm_camera_nbr,
        filename2
       );

       if ( !(sfm_camera2_ind < sfm_camera_nbr) ) {
          mvs10_error_handler("mvs_rectify_image_pair");
       }

       fprintf(stdout,"Corresponding sfm image pair : %d and %d ...\n",
        sfm_camera1_ind,sfm_camera2_ind);

       /*
       We have to be careful here because
       the cameras may have to be swapped to get the match file
       */

       if ( sfm_camera1_ind < sfm_camera2_ind ) {

          /*
          Match file should be there as is
          */

          /*
          sprintf(filename_match,"good_match_%d_%d.sfm",
           sfm_camera1_ind,sfm_camera2_ind);
          */
          sprintf(filename_match,"best_match_%d_%d.sfm",
           sfm_camera1_ind,sfm_camera2_ind);

          /*
          Match file better be there even if empty!
          */

          fp= fopen(filename_match,"r");

          if ( fp == 0 ) {
             /*
             fprintf(stdout,"good_match_%d_%d.sfm is not in the current directory",
              sfm_camera1_ind,sfm_camera2_ind);
             */
             fprintf(stdout,"best_match_%d_%d.sfm is not in the current directory",
              sfm_camera1_ind,sfm_camera2_ind);
             return 1;
          }

          sfm_read_matches(
           filename_match,
           &match_arr,
           &match_nbr
          );

          sfm_print_matches(
           "match.txt",
           match_arr,
           match_nbr
          );

          if ( match_nbr > 0 )
           free(match_arr);

          /*
          If at some point we also need the fundamental matrix,
          the F file should be there as is
          */
       }

       if ( sfm_camera2_ind < sfm_camera1_ind ) {

          /*
          Match file should be there
          but for the swapped camera pair
          */

          /*
          sprintf(filename_match,"good_match_%d_%d.sfm",
           sfm_camera2_ind,sfm_camera1_ind);
          */
          sprintf(filename_match,"best_match_%d_%d.sfm",
           sfm_camera2_ind,sfm_camera1_ind);

          /*
          Match file better be there even if empty!
          */

          fp= fopen(filename_match,"r");

          if ( fp == 0 ) {
             /*
             fprintf(stdout,"good_match_%d_%d.sfm is not in the current directory",
              sfm_camera2_ind,sfm_camera1_ind);
             */
             fprintf(stdout,"best_match_%d_%d.sfm is not in the current directory",
              sfm_camera2_ind,sfm_camera1_ind);
             return 1;
          }

          sfm_read_matches(
           filename_match,
           &match_arr,
           &match_nbr
          );

          mvs_swap_matches(
           match_arr,
           match_nbr
          );

          sfm_print_matches(
           "match.txt",
           match_arr,
           match_nbr
          );

          if ( match_nbr > 0 )
           free(match_arr);

          /*
          If at some point we also need the fundamental matrix,
          the F file should be there
          but for the swapped camera pair
          which means F would probably need to be inverted
          */
       }

       /*
       Discard the camera pair if there are too few matches
       */

       if ( match_nbr < min_match_nbr ) {
          fprintf(stdout,"Camera pair is discarded due to too few matches\n");
          continue;
       }

       /*
       Rectify the image pair
       The rectified images are stored on disk:
       image1_rect.jpg
       image2_rect.jpg
       */

       fprintf(stdout,"Rectifying the image pair ...\n");

       err_flag= mvs_rectify_image_pair(
        filename1,
        filename2,
        "match.txt",
        max_mean_error
       );

       fprintf(stdout,"Rectifying the image pair ... done.\n");

       if ( err_flag == 1 ) {
          fprintf(stdout,"Could not rectify image pair (camera pair is ignored)\n");
          continue;
       }

       /*
       Homography H1 is stored in file "H1.txt"
       We are gonna store it in a mvs file
       */

       fp= fopen("H1.txt","r");
       for ( i= 0 ; i< 3 ; i++ ) {
          for ( j= 0 ; j< 3 ; j++ ) {
             fscanf(fp,"%lg",&H1[i*3+j]);
          }
       }
       fclose(fp);

       sprintf(filename,"H1_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"w");
       for ( i= 0 ; i< 3 ; i++ ) {
          for ( j= 0 ; j< 3 ; j++ ) {
             fprintf(fp,"%g ",H1[i*3+j]);
          }
       }
       fclose(fp);

       /*
       Homography H2 is stored in file "H2.txt"
       We are gonna store it in a mvs file
       */

       fp= fopen("H2.txt","r");
       for ( i= 0 ; i< 3 ; i++ ) {
          for ( j= 0 ; j< 3 ; j++ ) {
             fscanf(fp,"%lg",&H2[i*3+j]);
          }
       }
       fclose(fp);

       sprintf(filename,"H2_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"w");
       for ( i= 0 ; i< 3 ; i++ ) {
          for ( j= 0 ; j< 3 ; j++ ) {
             fprintf(fp,"%g ",H2[i*3+j]);
          }
       }
       fclose(fp);

       /*
       Min and max disparities are stored in "disp_range.txt"
       We are gonna store them in a mvs file
       */

       fp= fopen("disp_range.txt","r");

       fscanf(fp,"%d %d",
        &min_d,&max_d);

       fclose(fp);

       sprintf(filename,"disp_range_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"w");
       fprintf(fp,"%d %d",min_d,max_d);
       fclose(fp);

       /*
       Get some stats about this camera pair
       from the matches in the match file
       match_nbr = number of matches
       avg_angle = average separation angle
       */

       mvs_get_camera_pair_stats_from_match_file(
        image_filename_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        threedpoint_arr,
        threedpoint_nbr,
        camera1_ind,
        camera2_ind,
        "match.txt", 
        &match_nbr,
        &avg_angle,
        &dist
       );

       fprintf(stdout,"Number of matches (from match file) = %d\n",
        match_nbr);

       fprintf(stdout,"Average separation angle (from match file) = %g\n",
        avg_angle);

       fprintf(stdout,"Distance between the camera centers (from match file) = %g\n",
        dist);

       /*
       Do not compute the disparity map
       if the average separation angle is below the minimum allowed
       */

       if ( avg_angle < min_avg_separation_angle ) {
          fprintf(stdout,"Camera pair is discarded due to average separation angle too small\n");
          continue;
       } 

       /*
       Compute the disparity map
       for the rectified image pair
       */

       fprintf(stdout,"Computing the disparity map ...\n");

       mvs_compute_disparity_map(
        radius,
        alpha,
        max_cost_color,
        max_cost_gradient,
        epsilon_int,
        disp_tol,
        ds_factor,
        &disp1_arr,
        &disp2_arr,
        &occ1_arr,
        &occ2_arr
       );

       fprintf(stdout,"Computing the disparity map ... done.\n");

       /*
       Write the disparities onto a file
       */

       sprintf(filename,"disp1_arr_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"w");
       for ( i= 0 ; i< height ; i++ ) {
          for ( j= 0 ; j< width ; j++ ) {
             fprintf(fp,"%d\n",disp1_arr[i*width+j]);
          }
       }
       fclose(fp);

       /*
       Write the occluded pixels onto a file
       */

       sprintf(filename,"occ1_arr_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"w");
       for ( i= 0 ; i< height ; i++ ) {
          for ( j= 0 ; j< width ; j++ ) {
             fprintf(fp,"%d\n",occ1_arr[i*width+j]);
          }
       }
       fclose(fp);

       /*
       Write the disparity map onto an image file
       */

       sprintf(filename,"disp1_%d_%d.jpg",
        camera1_ind,camera2_ind);

       normalized_disp1_arr= (int *)calloc(width*height,sizeof(int));

       normalize_image(
        disp1_arr,
        normalized_disp1_arr,
        width*height,
        min_d,
        max_d
       );

       err_flag= write_image(
        filename,
        normalized_disp1_arr,
        width,
        height
       );

       free(normalized_disp1_arr);

       /*
       Write the occlusion map onto an image file
       */

       sprintf(filename,"occ1_%d_%d.jpg",
        camera1_ind,camera2_ind);

       err_flag= write_image(
        filename,
        occ1_arr,
        width,
        height
       );

       /* 
       Free disp1_arr
       */

       free(disp1_arr);

       /*
       Free disp2_arr
       */

       free(disp2_arr);

       /*
       Free occ1_arr
       */

       free(occ1_arr);

       /*
       Free occ2_arr
       */

       free(occ2_arr);

       fprintf(stdout,"Processing image pair %d and %d ... done.\n",
        camera1_ind,camera2_ind);
    }
 }

 fprintf(stdout,"Rectifying the image pairs and computing the depth maps ... done.\n");

 /*
 Create file named "phase1.mvs"
 to indicate phase 1 has been completed
 */

 fp= fopen("phase1.mvs","w");
 fclose(fp);

 PHASE2:

 fprintf(stdout,"Adding the depth maps to the 3D reconstruction ...\n");

 /*
 Because of complications due to
 possibly having different features at the same pixel location,
 I think it's just easier to get rid of the 3D points
 Recall that sift can generate 2 or more features at the same pixel location
 (they have different gradient directions and magnitudes)
 */

 /*
 Free threedpoint_arr
 */

 sfm_free_threedpoints(
  threedpoint_arr,
  threedpoint_nbr
 );

 threedpoint_arr= 0;
 threedpoint_nbr= 0;

 /*
 Print the actual number of 3D points
 */

 nbr= mvs_get_threedpoints_nbr(
  threedpoint_arr,
  threedpoint_nbr
 );

 fprintf(stdout,"Number of 3D points = %d\n",
  nbr);

 for ( camera1_ind= 0 ;
       camera1_ind< camera_nbr ;
       camera1_ind++ ) {

    /*
    Get filename for image
    */

    strcpy(filename1,image_filename_arr[camera1_ind]);

    width1= width_arr[camera1_ind];
    height1= height_arr[camera1_ind];

    for ( camera2_ind= camera1_ind+1 ;
          camera2_ind< camera_nbr ;
          camera2_ind++ ) {

       fprintf(stdout,"Processing image pair %d and %d ...\n",
        camera1_ind,camera2_ind);

       /*
       Get filename for image
       */

       strcpy(filename2,image_filename_arr[camera2_ind]);

       width2= width_arr[camera2_ind];
       height2= height_arr[camera2_ind];

       if ( width2 != width1 ) {
          mvs10_error_handler("main");
       }
       if ( height2 != height1 ) {
          mvs10_error_handler("main");
       }

       width= width1; 
       height= height1; 

       /*
       Do not do anything if the disparity map has not been computed
       If the disparities have not been stored,
       the disparity map was not computed
       */

       sprintf(filename,"disp1_arr_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"r");
       if ( fp == 0 ) {
          fprintf(stdout,"No disparity map was computed for this camera pair\n");
          continue;
       }
       fclose(fp);

       /*
       Recover H1 and store in file "H1.txt"
       */

       sprintf(filename,"H1_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"r");
       for ( i= 0 ; i< 3 ; i++ ) {
          for ( j= 0 ; j< 3 ; j++ ) {
             fscanf(fp,"%lg",&H1[i*3+j]);
          }
       }
       fclose(fp);

       fp= fopen("H1.txt","w");
       for ( i= 0 ; i< 3 ; i++ ) {
          for ( j= 0 ; j< 3 ; j++ ) {
             fprintf(fp,"%g ",H1[i*3+j]);
          }
       }
       fclose(fp);

       /*
       Recover H2 and store in file "H2.txt"
       */

       sprintf(filename,"H2_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"r");
       for ( i= 0 ; i< 3 ; i++ ) {
          for ( j= 0 ; j< 3 ; j++ ) {
             fscanf(fp,"%lg",&H2[i*3+j]);
          }
       }
       fclose(fp);

       fp= fopen("H2.txt","w");
       for ( i= 0 ; i< 3 ; i++ ) {
          for ( j= 0 ; j< 3 ; j++ ) {
             fprintf(fp,"%g ",H2[i*3+j]);
          }
       }
       fclose(fp);

       /*
       Recover disp1_arr
       */

       disp1_arr= (int *)calloc(width*height,sizeof(int));

       sprintf(filename,"disp1_arr_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"r");
       for ( i= 0 ; i< height ; i++ ) {
          for ( j= 0 ; j< width ; j++ ) {
             fscanf(fp,"%d",&disp1_arr[i*width+j]);
          }
       }
       fclose(fp);

       /*
       Recover occ1_arr
       */

       occ1_arr= (int *)calloc(width*height,sizeof(int));

       sprintf(filename,"occ1_arr_%d_%d.mvs",
        camera1_ind,camera2_ind);

       fp= fopen(filename,"r");
       for ( i= 0 ; i< height ; i++ ) {
          for ( j= 0 ; j< width ; j++ ) {
             fscanf(fp,"%d",&occ1_arr[i*width+j]);
          }
       }
       fclose(fp);

       /*
       For each image, for each pixel,
       need to be able to get to the threedpoint if there is one
       */

       mvs_init_pixel_to_threedpoint_arr(
        image_filename_arr,
        width_arr,
        height_arr,
        focal_length_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        threedpoint_arr,
        threedpoint_nbr,
        &pixel2threedpoint_arr
       );

       /*
       Add disparity map to the 3D reconstruction
       */

       fprintf(stdout,"Adding the disparity map to the 3D scene ...\n");

       mvs_add_disparity_map(
        image_filename_arr,
        width_arr,
        height_arr,
        focal_length_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        filename1,
        filename2,
        camera1_ind,
        camera2_ind,
        disp1_arr,
        occ1_arr,
        sampling_step,
        &threedpoint_arr,
        &threedpoint_nbr,
        &pixel2threedpoint_arr
       );

       fprintf(stdout,"Adding the disparity map to the 3D scene ... done.\n");

       /*
       Free disp1_arr
       */

       free(disp1_arr);

       /*
       Free occ1_arr
       */

       free(occ1_arr);

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

       /*
       Print the actual number of 3D points
       */

       nbr= mvs_get_threedpoints_nbr(
        threedpoint_arr,
        threedpoint_nbr
       );

       fprintf(stdout,"Number of 3D points = %d\n",
        nbr);

       /*
       Check the 3D points
       */

       mvs_check_threedpoints(
        image_filename_arr,
        width_arr,
        height_arr,
        focal_length_arr,
        camera_nbr,
        threedpoint_arr,
        threedpoint_nbr
       );

       /*
       Recompute the 3D points
       from all image points
       */

       /*
       fprintf(stdout,"Recomputing the 3D points ...\n");

       sfm_recompute_threedpoints(
        image_filename_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        threedpoint_arr,
        threedpoint_nbr
       );

       fprintf(stdout,"Recomputing the 3D points ... done.\n");
       */

       /*
       Remove spurious image points
       */

       /*
       fprintf(stdout,"Removing spurious image points (reprojection error) ...\n");

       mvs_remove_spurious_imagepoints(
        image_filename_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        &threedpoint_arr,
        &threedpoint_nbr,
        max_reproj_error
       );

       fprintf(stdout,"Removing spurious image points (reprojection error) ... done.\n");
       */

       /*
       Print the actual number of 3D points
       */

       /*
       nbr= mvs_get_threedpoints_nbr(
        threedpoint_arr,
        threedpoint_nbr
       );

       fprintf(stdout,"Number of 3D points = %d\n",
        nbr);
       */

       /*
       Remove spurious 3D points
       */

       /*
       fprintf(stdout,"Removing spurious 3D points ...\n");

       min_imagepoint_nbr2= 2;

       mvs_remove_spurious_threedpoints(
        image_filename_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        &threedpoint_arr,
        &threedpoint_nbr,
        min_separation_angle,
        min_imagepoint_nbr2
       );

       fprintf(stdout,"Removing spurious 3D points ... done.\n");
       */

       /*
       Print the actual number of 3D points
       */

       /*
       nbr= mvs_get_threedpoints_nbr(
        threedpoint_arr,
        threedpoint_nbr
       );

       fprintf(stdout,"Number of 3D points = %d\n",
        nbr);
       */

       /*
       Perform Bundle Adjustment on all 3D points
       */

       /*
       fprintf(stdout,"Performing Bundle Adjustment on all 3D points ...\n");

       mvs_pba_bundle_adjustment(
        image_filename_arr,
        focal_length_arr,
        camera_nbr,
        camera_matrix_arr,
        camera_matrix_nbr,
        threedpoint_arr,
        threedpoint_nbr
       );

       fprintf(stdout,"Performing Bundle Adjustment on all 3D points ... done.\n");
       */

       fprintf(stdout,"Processing image pair %d and %d ... done.\n",
        camera1_ind,camera2_ind);
    }
 }

 fprintf(stdout,"Adding the depth maps to the 3D reconstruction ... done.\n");

 /*
 Recompute the 3D points
 from all image points
 */

 fprintf(stdout,"Recomputing the 3D points ...\n");

 sfm_recompute_threedpoints(
  image_filename_arr,
  camera_nbr,
  camera_matrix_arr,
  camera_matrix_nbr,
  threedpoint_arr,
  threedpoint_nbr
 );

 fprintf(stdout,"Recomputing the 3D points ... done.\n");

 /*
 Remove spurious image points
 */

 fprintf(stdout,"Removing spurious image points (reprojection error) ...\n");

 mvs_remove_spurious_imagepoints(
  image_filename_arr,
  camera_nbr,
  camera_matrix_arr,
  camera_matrix_nbr,
  &threedpoint_arr,
  &threedpoint_nbr,
  max_reproj_error
 );

 fprintf(stdout,"Removing spurious image points (reprojection error) ... done.\n");

 /*
 Print the actual number of 3D points
 */

 nbr= mvs_get_threedpoints_nbr(
  threedpoint_arr,
  threedpoint_nbr
 );

 fprintf(stdout,"Number of 3D points = %d\n",
  nbr);

 /*
 Remove spurious 3D points
 */

 fprintf(stdout,"Removing spurious 3D points ...\n");

 mvs_remove_spurious_threedpoints(
  image_filename_arr,
  camera_nbr,
  camera_matrix_arr,
  camera_matrix_nbr,
  &threedpoint_arr,
  &threedpoint_nbr,
  min_separation_angle,
  min_imagepoint_nbr
 );

 fprintf(stdout,"Removing spurious 3D points ... done.\n");

 /*
 Print the actual number of 3D points
 */

 nbr= mvs_get_threedpoints_nbr(
  threedpoint_arr,
  threedpoint_nbr
 );

 fprintf(stdout,"Number of 3D points = %d\n",
  nbr);

 /*
 Compute and print out the average and maximum reprojection error
 */

 sfm_print_reprojection_error_threedpoints(
  image_filename_arr,
  camera_nbr,
  camera_matrix_arr,
  camera_matrix_nbr,
  threedpoint_arr,
  threedpoint_nbr
 );

 /*
 Perform Bundle Adjustment on all 3D points
 */

 fprintf(stdout,"Performing Bundle Adjustment on all 3D points ...\n");

 mvs_pba_bundle_adjustment(
  image_filename_arr,
  focal_length_arr,
  camera_nbr,
  camera_matrix_arr,
  camera_matrix_nbr,
  threedpoint_arr,
  threedpoint_nbr
 );

 fprintf(stdout,"Performing Bundle Adjustment on all 3D points ... done.\n");

 /*
 Compute and print out the average and maximum reprojection error
 */

 sfm_print_reprojection_error_threedpoints(
  image_filename_arr,
  camera_nbr,
  camera_matrix_arr,
  camera_matrix_nbr,
  threedpoint_arr,
  threedpoint_nbr
 );

 /*
 Print the output files
 */

 sfm_print_threedpoints_in(
  "duh.in",
  threedpoint_arr,
  threedpoint_nbr
 );

 /*
 sfm_print_threedpoints_nvm(
  "duh.nvm",
  image_filename_arr,
  focal_length_arr,
  camera_nbr,
  camera_matrix_arr,
  camera_matrix_nbr,
  threedpoint_arr,
  threedpoint_nbr
 );
 */

 sfm_print_threedpoints_ply(
  "duh.ply",
  threedpoint_arr,
  threedpoint_nbr
 );

 sfm_print_threedpoints_gif(
  image_filename_arr,
  focal_length_arr,
  camera_nbr,
  camera_matrix_arr,
  camera_matrix_nbr,
  threedpoint_arr,
  threedpoint_nbr,
  radius_gif,
  max_theta_deg
 );

 /*
 Free sfm_image_filename_arr
 */

 for ( camera_ind= 0 ;
       camera_ind< sfm_camera_nbr ;
       camera_ind++ ) {
    free(sfm_image_filename_arr[camera_ind]);
 }
 if ( sfm_camera_nbr > 0 )
  free(sfm_image_filename_arr);

 /*
 Free image_filename_arr
 */

 for ( camera_ind= 0 ;
       camera_ind< camera_nbr ;
       camera_ind++ ) {
    free(image_filename_arr[camera_ind]);
 }
 if ( camera_nbr > 0 )
  free(image_filename_arr);

 /*
 Free width_arr
 */

 if ( camera_nbr > 0 )
  free(width_arr);

 /*
 Free height_arr
 */

 if ( camera_nbr > 0 )
  free(height_arr);

 /*
 Free focal_length_arr
 */

 if ( camera_nbr > 0 )
  free(focal_length_arr);

 /*
 Free threedpoint_arr
 */

 sfm_free_threedpoints(
  threedpoint_arr,
  threedpoint_nbr
 );

 /*
 Free camera_matrix_arr
 */

 if ( camera_matrix_nbr > 0 )
  free(camera_matrix_arr);

 return 0;

}
