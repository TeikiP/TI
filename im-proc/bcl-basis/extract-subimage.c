#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 6
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  int i = atoi(argv[1]);
  int j = atoi(argv[2]);
  unsigned int rows = atoi(argv[3]);
  unsigned int cols = atoi(argv[4]);

  pnm ims  = pnm_load(argv[5]);
  pnm imd  = pnm_new(cols, rows, PnmRawPpm); 

  for(size_t width=0; width<rows; width++){
    for(size_t height=0; height<cols; height++){
      for(size_t k=0; k<3; k++){

	const unsigned short val = pnm_get_component(ims, width+i, height+j, k);
	pnm_set_component(imd, width+i, height+j, k, val);
      }
    }
  }

  pnm_save(imd, PnmRawPpm, argv[6]);

  return EXIT_SUCCESS;
}
