/**
 * @file copy.c
 * @brief Upscales a specific image
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bcl.h>
#include <fft.h>


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

  /* Initialisation. */
  int factor = atoi(argv[1]);

  /* Chargement de l'image source. */
  pnm ims = pnm_load(argv[2]);

  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);

  size_t newWidth  = width * factor;
  size_t newHeight = height * factor;

  /* Application de la transformation de Fourier. */
  unsigned short * sourceChannel = pnm_get_channel(ims, NULL, 0);

  fftw_complex * forwardCplx = forward(height, width, sourceChannel);
  fftw_complex * paddingCplx = fftw_malloc(sizeof(fftw_complex) * newWidth * newHeight);

  /* Padding. */
  const unsigned int topPadding = height/2 * (factor-1);
  const unsigned int botPadding = height/2 * (factor+1);
  const unsigned int lefPadding = width/2 * (factor-1);
  const unsigned int rigPadding = width/2 * (factor+1);

  for (unsigned int i=0; i<newHeight; i++)
    for (unsigned int j=0; j<newWidth; j++) {
      if (i < topPadding || i >= botPadding || j < lefPadding || j >= rigPadding) {
        paddingCplx[i*newWidth+j] = 0;
      }

      else
        paddingCplx[i*newWidth+j] = forwardCplx[(i-topPadding)*width + j-lefPadding];
    }

  /* Application de la transformation de Fourier inverse. */
  unsigned short * resultChannel = backwardZoom(newHeight, newWidth, paddingCplx, factor);

  /* Construction de l'image de destination. */
  pnm imd  = pnm_new(newWidth, newHeight, PnmRawPpm);

  for(size_t k=0; k<3; k++)
    pnm_set_channel(imd, resultChannel, k);

  /* Enregistrer l'image de destination. */
  pnm_save(imd, PnmRawPpm, argv[3]);

  /* Liberation de la memoire. */
  fftw_free(forwardCplx);
  fftw_free(paddingCplx);

  free(sourceChannel);
  free(resultChannel);

  pnm_free(ims);
  pnm_free(imd);

  return EXIT_SUCCESS;
}
