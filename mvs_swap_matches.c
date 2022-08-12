#include "header.h"

void mvs_swap_matches(
 match_struct *match_arr,
 int match_nbr
)

{

 int match_ind;
 match_struct match;
 int feat1_ind;
 double x1;
 double y1;
 int feat2_ind;
 double x2;
 double y2;

 for ( match_ind= 0; match_ind< match_nbr ; match_ind++ ) {
    match= match_arr[match_ind];

    feat1_ind= match.feat1_ind; 
    x1= match.x1;
    y1= match.y1;
    feat2_ind= match.feat2_ind; 
    x2= match.x2;
    y2= match.y2;

    match.feat1_ind= feat2_ind;
    match.x1= x2;
    match.y1= y2;
    match.feat2_ind= feat1_ind;
    match.x2= x1;
    match.y2= y1;

    match_arr[match_ind]= match;
 }

}
