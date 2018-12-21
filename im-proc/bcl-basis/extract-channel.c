#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<num> <ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 3
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  int num = atoi(argv[1]);

  pnm ims  = pnm_load(argv[2]);
  pnm imd  = pnm_new(pnm_get_width(ims), pnm_get_height(ims), PnmRawPpm); 


  unsigned short *channel = pnm_get_channel(ims, NULL, num);

  for(size_t k=0; k<3; k++)
    pnm_set_channel(imd, channel, k);

  pnm_save(imd, PnmRawPpm, argv[3]);
  free(channel);

  return EXIT_SUCCESS;
}
