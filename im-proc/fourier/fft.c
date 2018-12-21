#include <float.h>
#include <stdlib.h>
#include <math.h>

#include <fft.h>

fftw_complex *
forward(int rows, int cols, unsigned short* g_img)
{
  /* Initialisation. */
  unsigned long size = rows * cols;

  fftw_complex * sourceImg = fftw_malloc(size * sizeof(fftw_complex));
  fftw_complex * outputImg = fftw_malloc(size * sizeof(fftw_complex));

  /* Conversion en complexe. */
  for(unsigned long i=0; i<size; i++) {
    sourceImg[i] = g_img[i];
  }

  /* Fourier. */
  fftw_plan forwardPlan = fftw_plan_dft_2d(rows, cols, sourceImg, outputImg, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(forwardPlan);
  fftw_destroy_plan(forwardPlan);

  /* Liberation. */
  fftw_free(sourceImg);

  return outputImg;
}

unsigned short *
backward(int rows, int cols, fftw_complex* freq_repr)
{
  /* Initialisation. */
  unsigned long size = rows * cols;
  double norm = 1 / (double) size;

  fftw_complex * outputImg = fftw_malloc(size * sizeof(fftw_complex));
  unsigned short * backwardImg = malloc(size * sizeof(unsigned short));

  fftw_plan forwardPlan = fftw_plan_dft_2d(rows, cols, freq_repr, outputImg, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(forwardPlan);
  fftw_destroy_plan(forwardPlan);

  /* Extraction de la partie reelle et normalisation. */
  for(unsigned long i=0; i<size; i++)
    backwardImg[i] = norm * creal(outputImg[i]);

  /* Liberation memoire. */
  fftw_free(outputImg);

  return backwardImg;
}

void
freq2spectra(int rows, int cols, fftw_complex* freq_repr, float* as, float* ps) 
{
  /* Initialisation. */
  unsigned long size = rows * cols;

  /* Extraction de l'amplitude et de la phase. */
  for(unsigned long i=0; i<size; i++) {
    as[i] = sqrt(pow(creal(freq_repr[i]), 2) + pow(cimag(freq_repr[i]), 2));
    ps[i] = atan2(cimag(freq_repr[i]), creal(freq_repr[i]));
  }

}

void 
spectra2freq(int rows, int cols, float* as, float* ps, fftw_complex* freq_repr)
{
  /* Initialisation. */
  unsigned long size = rows * cols;

  /* Reconstruction du complexe. */
  for(unsigned long i=0; i<size; i++)
    freq_repr[i] = as[i] * cos(ps[i]) + I * as[i] * sin(ps[i]);

}
