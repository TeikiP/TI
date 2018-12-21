#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<ims0> <ims1> <ims2> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 4
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  pnm ims[]  = {pnm_load(argv[1]), pnm_load(argv[2]), pnm_load(argv[3])};
  pnm imd  = pnm_new(pnm_get_width(ims[0]), pnm_get_height(ims[0]), PnmRawPpm); 
 
  for(size_t k=0; k<3; k++) {
    unsigned short *channel = pnm_get_channel(ims[k], NULL, k);

    pnm_set_channel(imd, channel, k);

    free(channel);
    pnm_free(ims[k]);
  }

  pnm_save(imd, PnmRawPpm, argv[4]);
  pnm_free(imd);

  return EXIT_SUCCESS;
}
