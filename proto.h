void mvs10_error_handler(
 char* from
);

void mvs_add_disparity_map(
 char **image_filename_arr,
 int *width_arr,
 int *height_arr,
 double *focal_length_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 char *filename1,
 char *filename2,
 int camera1_ind,
 int camera2_ind,
 int *disp1_arr,
 int *occ1_arr,
 int sampling_step,
 sfm_threedpoint_struct **pthreedpoint_arr,
 int *pthreedpoint_nbr,
 int ***ppixel2threedpoint_arr
);

void mvs_add_imagepoint_to_threedpoint(
 int inp_camera_ind,
 int inp_feat_ind,
 double inp_x,
 double inp_y,
 int inp_pixel,
 int threedpoint_ind,
 int **pixel2threedpoint_arr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr
);

void mvs_add_match(
 char **image_filename_arr,
 int *width_arr,
 int *height_arr,
 double *focal_length_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 char *filename1,
 char *filename2,
 int *image1_arr,
 int *image2_arr,
 int camera1_ind,
 int camera2_ind,
 int i1,
 int j1,
 int i2,
 int j2,
 sfm_threedpoint_struct **pthreedpoint_arr,
 int *pthreedpoint_nbr,
 int ***ppixel2threedpoint_arr
);

void mvs_check_threedpoints(
 char **image_filename_arr,
 int *width_arr,
 int *height_arr,
 double *focal_length_arr,
 int camera_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr
);

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
);

double mvs_compute_reprojection_error(
 char **image_filename_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 int camera_ind,
 double x,
 double y,
 double xyz[3]
);

void mvs_compute_t_from_R_C(
 double R[3*3],
 double C[3],
 double t[3]
);

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
);

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
);

int mvs_get_sfm_camera_ind(
 char **sfm_image_filename_arr,
 int camera_nbr,
 char *image_filename
);

int mvs_get_threedpoints_nbr(
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr
);

void mvs_init_pixel_to_threedpoint_arr(
 char **image_filename_arr,
 int *width_arr,
 int *height_arr,
 double *focal_length_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr,
 int ***ppixel2threedpoint_arr
);

void mvs_lbfgs_bundle_adjustment(
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr,
 int lbfgs_max_iterations
);

void mvs_merge_threedpoints(
 char **image_filename_arr,
 int *width_arr,
 int *height_arr,
 int inp_threedpoint_ind1,
 int inp_threedpoint_ind2,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr,
 int **pixel2threedpoint_arr
);

void mvs_pba_bundle_adjustment(
 char **image_filename_arr,
 double *focal_length_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct *threedpoint_arr,
 int threedpoint_nbr
);

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
);

int mvs_rectify_image_pair(
 char *filename1,
 char *filename2,
 char *filename_match,
 double max_mean_error
);

void mvs_remove_spurious_imagepoints(
 char **image_filename_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct **pthreedpoint_arr,
 int *pthreedpoint_nbr,
 double max_reproj_error
);

void mvs_remove_spurious_threedpoints(
 char **image_filename_arr,
 int camera_nbr,
 sfm_camera_matrix_struct *camera_matrix_arr,
 int camera_matrix_nbr,
 sfm_threedpoint_struct **pthreedpoint_arr,
 int *pthreedpoint_nbr,
 double min_separation_angle,
 int min_imagepoint_nbr
);

void mvs_swap_matches(
 match_struct *match_arr,
 int match_nbr
);
