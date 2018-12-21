/**
 * @file test-fft.c
 * @brief test the behaviors of functions in fft module
 *
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fft.h>

/**
 * @brief saves a pnm to a new file named "modif name"
 * @param pnm imd, the pnm containing information about the image
 * @param const char* name, the input image file name
 * @param const char* prefix, the modification applied to the image
 */
void
make_new_file(pnm imd, const char* name, const char* prefix)
{
  char* newName = malloc(sizeof(char) * (strlen(name) + strlen(prefix)));

  strcpy(newName, prefix);
  strcat(newName, name);

  pnm_save(imd, PnmRawPpm, newName);

  free(newName);
}


/**
 * @brief test the forward and backward functions
 * @param char* name, the input image file name
 */
void
test_forward_backward(char* name)
{
  fprintf(stderr, "test_forward_backward: ");


  /* Chargement de l'image source. */
  pnm ims  = pnm_load(name);

  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);


  /* Application de la transformation de Fourier. */
  unsigned short * channel = pnm_get_channel(ims, NULL, 0);

  fftw_complex * forwardCplx = forward(height, width, channel);

  unsigned short * resultImg = backward(height, width, forwardCplx);


  /* Construction de l'image de destination. */
  pnm imd  = pnm_new(width, height, PnmRawPpm);

  for(size_t k=0; k<3; k++)
    pnm_set_channel(imd, resultImg, k);

  /* Enregistrer l'image de destination. */
  make_new_file(imd, name, "FB-");

  /* Liberation de la memoire. */
  fftw_free(forwardCplx);

  free(channel);
  free(resultImg);

  pnm_free(ims);
  pnm_free(imd);

  fprintf(stderr, "OK\n");
}

/**
 * @brief test image reconstruction from of magnitude and phase spectrum
 * @param char *name: the input image file name
 */
void
test_reconstruction(char* name)
{
  fprintf(stderr, "test_reconstruction: ");

  /* Chargement de l'image source. */
  pnm ims  = pnm_load(name);

  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);

  /* Application de la transformation de Fourier. */
  unsigned short* channel = pnm_get_channel(ims, NULL, 0);

  fftw_complex* forwardCplx = forward(height, width, channel);
  fftw_complex* backwardCplx = fftw_malloc(height * width * sizeof(fftw_complex));

  /* Extraction des spectres. */
  float* as = malloc(width * height * sizeof(float));
  float* ps = malloc(width * height * sizeof(float));

  freq2spectra(height, width, forwardCplx, as, ps);

  spectra2freq(height, width, as, ps, backwardCplx);

  /* Application de la transformation inverse. */
  unsigned short * resultImg = backward(height, width, backwardCplx);

  /* Construction de l'image de destination. */
  pnm imd  = pnm_new(width, height, PnmRawPpm);

  for(size_t k=0; k<3; k++)
    pnm_set_channel(imd, resultImg, k);

  /* Enregistrer l'image de destination. */
  make_new_file(imd, name, "FB-ASPS-");

  /* Liberation de la memoire. */
  fftw_free(forwardCplx);
  fftw_free(backwardCplx);

  free(channel);
  free(resultImg);

  free(as);
  free(ps);

  pnm_free(ims);
  pnm_free(imd);

  fprintf(stderr, "OK\n");
}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void
test_display(char* name)
{
  fprintf(stderr, "test_display: ");

  /* Chargement de l'image source. */
  pnm ims  = pnm_load(name);

  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);

  unsigned short* channel = pnm_get_channel(ims, NULL, 0);
  pnm_free(ims);

  /* Recentrage. */
  for(size_t x=0; x<width; x++)
    for(size_t y=0; y<height; y++)
      if((x+y) % 2 == 1)
        channel[y+height*x] *= -1;

  /* Application de la transformation de Fourier. */
  fftw_complex* forwardCplx = forward(height, width, channel);
  free(channel);

  /* Extraction des spectres. */
  float* as = malloc(width * height * sizeof(float));
  float* ps = malloc(width * height * sizeof(float));

  freq2spectra(height, width, forwardCplx, as, ps);
  fftw_free(forwardCplx);

  /* Recherche de maximum. */
  float maxAS = 0.0;
  float maxPS = 0.0;

  for(size_t i=0; i<width*height; i++) {
    maxAS = as[i]>maxAS?as[i]:maxAS;
    maxPS = ps[i]>maxPS?ps[i]:maxPS;
  }

  /* Transformation non-lineaire. */
  unsigned short* asChannel = malloc(width * height * sizeof(unsigned short));
  unsigned short* psChannel = malloc(width * height * sizeof(unsigned short));

  for(size_t i=0; i<width*height; i++) {
    asChannel[i] = pow(as[i]/maxAS, .1) * 255.0f;

    if(asChannel[i] > 255)
      asChannel[i] = 255;
    
    psChannel[i] = (unsigned short) (ps[i] /3.141593f * (float) 255);

    if(psChannel[i] > 255)
      psChannel[i] = 255;
  }

  free(as);
  free(ps);

  /* Construction des images de destination. */
  pnm imdAS  = pnm_new(width, height, PnmRawPpm);
  pnm imdPS  = pnm_new(width, height, PnmRawPpm);

  for(size_t k=0; k<3; k++) {
    pnm_set_channel(imdAS, asChannel, k);
    pnm_set_channel(imdPS, psChannel, k);
  }

  free(asChannel);
  free(psChannel);

  /* Enregistrer les images de destination. */
  make_new_file(imdAS, name, "AS-");
  make_new_file(imdPS, name, "PS-");

  pnm_free(imdAS);
  pnm_free(imdPS);

  fprintf(stderr, "OK\n");
}

/**
 * @brief test the modification of magnitude by adding a periodic functions
          on both vertical and horizontal axis, and 
 *        construct output images
 * @param char* name, the input image file name
 */
void

test_add_frequencies(char* name)
{
  fprintf(stderr, "test_add_frequencies: ");
  (void)name;
  fprintf(stderr, "OK\n");
}

void
run(char* name)
{
  test_forward_backward(name);
  test_reconstruction(name);
  test_display(name);
  test_add_frequencies(name);
}

void 
usage(const char *s)
{
  fprintf(stderr, "Usage: %s <ims> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 1
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);
  run(argv[1]);
  return EXIT_SUCCESS;
}
