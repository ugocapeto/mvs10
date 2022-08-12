#include "header.h"
#include "proto.h"

void mvs10_error_handler(
 char* from
)

{

 fprintf(stderr,"FATAL ERROR in %s\n",from);
 /* exit(EXIT_FAILURE); */
 abort();

}
