#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 2
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  pnm ims  = pnm_load(argv[1]);

  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);

  pnm imd  = pnm_new(width, height, PnmRawPpm); 

  for(size_t i=0; i<width; i++) {
    for(size_t j=0; j<height; j++) {

      unsigned short val[3];

      for(size_t k=0; k<3; k++)
	val[k] = pnm_get_component(ims, i, j, k);

      unsigned short valMean = (val[0] + val[1] + val[2]) / 3;

      for(size_t k=0; k<3; k++)
        pnm_set_component(imd, i, j, k, valMean);
    }
  }

  pnm_save(imd, PnmRawPpm, argv[2]);

  pnm_free(ims);
  pnm_free(imd);

  return EXIT_SUCCESS;
}
