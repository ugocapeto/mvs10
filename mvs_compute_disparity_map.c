#include "header.h"

void mvs_compute_disparity_map(
 int radius,
 double alpha,
 double max_cost_color,
 double max_cost_gradient,
 int epsilon_int,
 int disp_tol,
 int ds_factor,
 int **pdisp1_arr,
 int **pdisp2_arr,
 int **pocc1_arr,
 int **pocc2_arr
)

{

 int width;
 int height;
 int err_flag;
 int *image1_arr;
 int width1;
 int height1;
 int *image2_arr;
 int width2;
 int height2;
 FILE *fp;
 int min_d;
 int max_d;
 int min_d1;
 int max_d1;
 int min_d2;
 int max_d2;
 int *disp1_arr;
 int *normalized_disp1_arr;
 int *disp2_arr;
 int *normalized_disp2_arr;
 int *occ1_arr;
 int *occ2_arr;
 double epsilon;
 int width_ds;
 int height_ds;
 int *image1_ds_arr;
 int *image2_ds_arr;
 int i_ds;
 int j_ds;
 int i;
 int j;
 int *disp1_ds_arr;
 int *disp2_ds_arr;
 int *occ1_ds_arr;
 int *occ2_ds_arr;
 int min_d_ds;
 int max_d_ds;
 int min_d1_ds;
 int max_d1_ds;
 int min_d2_ds;
 int max_d2_ds;
 int radius_ds;
 int *normalized_disp1_ds_arr;
 int *normalized_disp2_ds_arr;
 int pixel_ds;
 int pixel;
 int cind;
 int disp_tol_ds;

 epsilon= 255*255*pow(10,-epsilon_int);

 /*
 Load rectified image 1
 */

 err_flag= load_rgb_image(
  "image1_rect.jpg",
  &image1_arr,
  &width1,
  &height1
 );

 /*
 Load rectified image 2
 */

 err_flag= load_rgb_image(
  "image2_rect.jpg",
  &image2_arr,
  &width2,
  &height2
 );

 if ( width2 != width1 ) {
    mvs10_error_handler("mvs_compute_disparity_map");
 }
 if ( height2 != height1 ) {
    mvs10_error_handler("mvs_compute_disparity_map");
 }

 width= width1;
 height= height1;

 /*
 Get the min and max disparity
 They were stored when the images were rectified
 */

 fp= fopen("disp_range.txt","r");

 fscanf(fp,"%d %d",
  &min_d,&max_d);

 fclose(fp);

 min_d1= min_d;
 max_d1= max_d;
 min_d2=-max_d;
 max_d2=-min_d;

 /*
 Downsample the images by a factor of ds_factor
 */

 width_ds= (int)((double)width/(double)ds_factor);
 height_ds= (int)((double)height/(double)ds_factor);

 image1_ds_arr= (int *)calloc(width_ds*height_ds,3*sizeof(int));

 for ( i_ds= 0 ; i_ds< height_ds ; i_ds++ ) {
    i= ds_factor*i_ds;
    if ( i > height-1 ) {
       i= height-1;
    }
    for ( j_ds= 0 ; j_ds< width_ds ; j_ds++ ) {
       j= ds_factor*j_ds;
       if ( j > width-1 ) {
          j= width-1;
       }
       pixel_ds= i_ds*width_ds+j_ds;
       pixel= i*width+j;
       for ( cind= 0 ; cind< 3 ; cind++ )
        image1_ds_arr[3*pixel_ds+cind]= image1_arr[3*pixel+cind];
    }
 }
       
 image2_ds_arr= (int *)calloc(width_ds*height_ds,3*sizeof(int));

 for ( i_ds= 0 ; i_ds< height_ds ; i_ds++ ) {
    i= ds_factor*i_ds;
    if ( i > height-1 ) {
       i= height-1;
    }
    for ( j_ds= 0 ; j_ds< width_ds ; j_ds++ ) {
       j= ds_factor*j_ds;
       if ( j > width-1 ) {
          j= width-1;
       }
       pixel_ds= i_ds*width_ds+j_ds;
       pixel= i*width+j;
       for ( cind= 0 ; cind< 3 ; cind++ )
        image2_ds_arr[3*pixel_ds+cind]= image2_arr[3*pixel+cind];
    }
 }

 min_d_ds= (int)((double)min_d/(double)ds_factor);
 max_d_ds= (int)((double)max_d/(double)ds_factor);

 min_d1_ds= min_d_ds;
 max_d1_ds= max_d_ds;
 min_d2_ds=-max_d_ds;
 max_d2_ds=-min_d_ds;

 radius_ds= (int)((double)radius/(double)ds_factor);

 disp_tol_ds= (int)((double)disp_tol/(double)ds_factor);

 /*
 Get the disparity map 1
 */

 disp1_ds_arr= (int *)calloc(width_ds*height_ds,sizeof(int));

 fast_cost_volume_filter(
  width_ds,
  height_ds,
  image1_ds_arr,
  image2_ds_arr,
  min_d1_ds,
  max_d1_ds,
  radius_ds,
  alpha,
  max_cost_color,
  max_cost_gradient,
  epsilon,
  disp1_ds_arr
 );

 normalized_disp1_ds_arr= (int *)calloc(width_ds*height_ds,sizeof(int));

 normalize_image(
  disp1_ds_arr,
  normalized_disp1_ds_arr,
  width_ds*height_ds,
  min_d1_ds,
  max_d1_ds
 );

 err_flag= write_image(
  "disp1_ds.jpg",
  normalized_disp1_ds_arr,
  width_ds,
  height_ds
 );

 free(normalized_disp1_ds_arr);

 /*
 Get the disparity map 2
 */

 disp2_ds_arr= (int *)calloc(width_ds*height_ds,sizeof(int));

 fast_cost_volume_filter(
  width_ds,
  height_ds,
  image2_ds_arr,
  image1_ds_arr,
  min_d2_ds,
  max_d2_ds,
  radius_ds,
  alpha,
  max_cost_color,
  max_cost_gradient,
  epsilon,
  disp2_ds_arr
 );

 normalized_disp2_ds_arr= (int *)calloc(width_ds*height_ds,sizeof(int));

 normalize_image(
  disp2_ds_arr,
  normalized_disp2_ds_arr,
  width_ds*height_ds,
  min_d2_ds,
  max_d2_ds
 );

 err_flag= write_image(
  "disp2_ds.jpg",
  normalized_disp2_ds_arr,
  width_ds,
  height_ds
 );

 free(normalized_disp2_ds_arr);

 /*
 Detect occlusions in image 1
 */

 occ1_ds_arr= (int *)calloc(width_ds*height_ds,sizeof(int));

 detect_occlusion(
  width_ds,
  height_ds,
  disp1_ds_arr,
  disp2_ds_arr,
  min_d1_ds,
  max_d1_ds,
  disp_tol_ds,
  occ1_ds_arr
 );

 err_flag= write_image(
  "occ1_ds.jpg",
  occ1_ds_arr,
  width_ds,
  height_ds
 );

 /*
 Detect occlusions in image 2
 */

 occ2_ds_arr= (int *)calloc(width_ds*height_ds,sizeof(int));

 detect_occlusion(
  width_ds,
  height_ds,
  disp2_ds_arr,
  disp1_ds_arr,
  min_d2_ds,
  max_d2_ds,
  disp_tol_ds,
  occ2_ds_arr
 );

 err_flag= write_image(
  "occ2_ds.jpg",
  occ2_ds_arr,
  width_ds,
  height_ds
 );

 /*
 Upsample disp1_ds_arr
 */

 disp1_arr= (int *)calloc(width*height,sizeof(int));

 for ( i= 0 ; i< height ; i++ ) {
    i_ds= (int)((double)i/(double)ds_factor);
    if ( i_ds > height_ds-1 ) {
       i_ds= height_ds-1;
    }
    for ( j= 0 ; j< width ; j++ ) {
       j_ds= (int)((double)j/(double)ds_factor);
       if ( j_ds > width_ds-1 ) {
          j_ds= width_ds-1;
       }
       disp1_arr[i*width+j]= ds_factor*disp1_ds_arr[i_ds*width_ds+j_ds];
    }
 }

 normalized_disp1_arr= (int *)calloc(width*height,sizeof(int));

 normalize_image(
  disp1_arr,
  normalized_disp1_arr,
  width*height,
  min_d1,
  max_d1
 );

 err_flag= write_image(
  "disp1.jpg",
  normalized_disp1_arr,
  width,
  height
 );

 free(normalized_disp1_arr);

 /*
 Upsample disp2_ds_arr
 */

 disp2_arr= (int *)calloc(width*height,sizeof(int));

 for ( i= 0 ; i< height ; i++ ) {
    i_ds= (int)((double)i/(double)ds_factor);
    if ( i_ds > height_ds-1 ) {
       i_ds= height_ds-1;
    }
    for ( j= 0 ; j< width ; j++ ) {
       j_ds= (int)((double)j/(double)ds_factor);
       if ( j_ds > width_ds-1 ) {
          j_ds= width_ds-1;
       }
       disp2_arr[i*width+j]= ds_factor*disp2_ds_arr[i_ds*width_ds+j_ds];
    }
 }

 normalized_disp2_arr= (int *)calloc(width*height,sizeof(int));

 normalize_image(
  disp2_arr,
  normalized_disp2_arr,
  width*height,
  min_d2,
  max_d2
 );

 err_flag= write_image(
  "disp2.jpg",
  normalized_disp2_arr,
  width,
  height
 );

 free(normalized_disp2_arr);

 /*
 Upsample occ1_ds_arr
 */

 occ1_arr= (int *)calloc(width*height,sizeof(int));

 for ( i= 0 ; i< height ; i++ ) {
    i_ds= (int)((double)i/(double)ds_factor);
    if ( i_ds > height_ds-1 ) { 
       i_ds= height_ds-1;
    }
    for ( j= 0 ; j< width ; j++ ) {
       j_ds= (int)((double)j/(double)ds_factor);
       if ( j_ds > width_ds-1 ) {
          j_ds= width_ds-1;
       }
       occ1_arr[i*width+j]= occ1_ds_arr[i_ds*width_ds+j_ds];
    }
 }

 err_flag= write_image( 
  "occ1.jpg",
  occ1_arr,
  width,
  height
 );

 /*
 Upsample occ2_ds_arr
 */

 occ2_arr= (int *)calloc(width*height,sizeof(int));

 for ( i= 0 ; i< height ; i++ ) {
    i_ds= (int)((double)i/(double)ds_factor);
    if ( i_ds > height_ds-1 ) {
       i_ds= height_ds-1;
    }
    for ( j= 0 ; j< width ; j++ ) {
       j_ds= (int)((double)j/(double)ds_factor);
       if ( j_ds > width_ds-1 ) {
          j_ds= width_ds-1;
       }
       occ2_arr[i*width+j]= occ2_ds_arr[i_ds*width_ds+j_ds];
    }
 }

 err_flag= write_image(
  "occ2.jpg",
  occ2_arr,
  width,
  height
 );

 /*
 Free memory
 */

 free(image1_arr);
 free(image2_arr);
 free(image1_ds_arr);
 free(image2_ds_arr);
 free(disp1_ds_arr);
 free(disp2_ds_arr);
 free(occ1_ds_arr);
 free(occ2_ds_arr);

 (*pdisp1_arr)= disp1_arr;
 (*pdisp2_arr)= disp2_arr;
 (*pocc1_arr)= occ1_arr;
 (*pocc2_arr)= occ2_arr;

}
