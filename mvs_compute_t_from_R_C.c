#include "header.h"

void mvs_compute_t_from_R_C(
 double R[3*3],
 double C[3],
 double t[3]
)

/*
Compute t (translation) from R and C (camera center)
t=-R*C
*/

{

 double RC[3]; 
 int i;

 math_matrix_vector_product(
  R,
  3,
  3,
  C,
  3,
  RC
 );

 for ( i= 0 ; i< 3 ; i++ )
  t[i]=-RC[i];

}
