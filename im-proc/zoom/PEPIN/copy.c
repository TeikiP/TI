/**
 * @file copy.c
 * @brief Upscales a specific image
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bcl.h>


void 
usage(const char *s)
{
  fprintf(stderr, "Usage: %s <factor> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int 
main(int argc, char *argv[])
{
  /* Verification des arguments. */
  if (argc != PARAM+1) usage(argv[0]);

  /* Lecture du facteur de magnification. */
  int factor = atoi(argv[1]);

  /* Chargement de l'image source. */
  pnm ims  = pnm_load(argv[2]);

  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);

  /* Construction de l'image de destination. */
  pnm imd  = pnm_new(width*factor, height*factor, PnmRawPpm);

  /* Processus de magnification. */
  for(size_t i=0; i<width; i++) {
    for(size_t j=0; j<height; j++) {
      for(size_t k=0; k<3; k++) {
	      const unsigned short val = pnm_get_component(ims, i, j, k);

        for(int iOffset = 0; iOffset<factor; iOffset++)
	        for(int jOffset = 0; jOffset<factor; jOffset++)
	          pnm_set_component(imd, i*factor+iOffset, j*factor+jOffset, k, val);
      }
    }
  }

  /* Enregistrer l'image de destination. */
  pnm_save(imd, PnmRawPpm, argv[3]);

  pnm_free(ims);
  pnm_free(imd);

  return EXIT_SUCCESS;
}
