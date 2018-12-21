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
  fprintf(stderr, "Usage: %s <factor> <filter-name> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}


int
box(double x)
{
  int result = 0;
  
  if (x >= -.5 && x < .5)
    result = 1;
  
  return result;
}

double
tent(double x)
{
  double result = 0;
  
  if (x >= -1 && x <= 1)
    result = 1 - abs(x);
  
  return result;
}

double
bell(double x)
{
  double result = 0;
  
  if (abs(x) <= .5)
    result = -1 * x * x + 3 / 4;
    
  else if (abs(x) <= 1.5)
    result = .5 * (abs(x) - 1.5) * (abs(x) - 1.5);
  
  return result;
}

double
mitchell(double x)
{
  double result = 0;
  
  if (x >= -1 && x <= 1)
    result = 7/6 * abs(x) * abs(x) * abs(x) - 2 * x * x + 8/9;
    
  else if ((x >= -2 && x <= -1) || (x >= 1 && x <= 2))
    result = -7/18 * abs(x) * abs(x) * abs(x) + 2 * x * x - 10 / 3 * abs(x) + 16/9;
  
  return result;
}

pnm
transpose(pnm ims) {
  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);

  pnm imt  = pnm_new(height, width, PnmRawPpm);
  
  for (unsigned int i=0; i<width; i++)
    for (unsigned int j=0; j<height; j++)
      for (unsigned int channel = 0; channel<3; channel++)
        pnm_set_component(imt, j, i, channel, pnm_get_component(ims, i, j, channel));
      
  return imt;
}

pnm
interpolation(pnm ims, int factor, const char *filter)
{
  /* Parametres. */
  char c = '0';
  const char *boxStr = "box";
  const char *tentStr = "tent";
  const char *bellStr = "bell";
  const char *mitchellStr = "mitchell";
  
  if (strcmp(filter, boxStr) == 0)
    c = 'b';
  
  else if (strcmp(filter, tentStr) == 0)
    c = 't';
    
  else if (strcmp(filter, bellStr) == 0)
    c = 'l';
    
  else if (strcmp(filter, mitchellStr) == 0)
    c = 'm';
    
  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);

  /* Construction de l'image de destination. */
  size_t newWidth = width*factor;
  size_t newHeight = height*factor;
  pnm imd = pnm_new(newWidth, newHeight, PnmRawPpm);

  /* Interpolation. */
  for (unsigned int i = 0; i<height; i++) {
    for (unsigned int jp = 0; jp<newWidth-1; jp++) {
      double j = (double)jp / (double)factor;
      double WF = .5;
      
      unsigned int left = j - WF;
      
      unsigned int right = j + WF;
      if (right > newWidth-1)
        right = newWidth-1;
      
      unsigned short s[3] = {0,0,0};
      
      for (unsigned int k = left; k<right; k++)
        for (unsigned int channel = 0; channel<3; channel++) {
            switch (c) {
              case 'b':
                s[channel] += pnm_get_component(ims, i, k, channel) * box(k-j);
                break;
                
              case 't':
                s[channel] += pnm_get_component(ims, i, k, channel) * tent(k-j);
                break;
                
              case 'l':
                s[channel] += pnm_get_component(ims, i, k, channel) * bell(k-j);
                break;
                
              case 'm':
                s[channel] += pnm_get_component(ims, i, k, channel) * mitchell(k-j);
                break;
                
              default:
                s[channel] += pnm_get_component(ims, i, k, channel) * box(k-j);
                break;
            }
          }
      
      for (unsigned int channel = 0; channel<3; channel++)
        pnm_set_component(imd, i, jp, channel, s[channel]);
    }
  }
  
  return imd;
}

pnm
interpolationSansAgrand(pnm ims, int factor, const char *filter)
{
  /* Parametres. */
  double WF = .5;
  char c = '0';
  const char *boxStr = "box";
  const char *tentStr = "tent";
  const char *bellStr = "bell";
  const char *mitchellStr = "mitchell";
  
  if (strcmp(filter, boxStr) == 0) {
    c = 'b';
    WF = .5;
  }
  
  else if (strcmp(filter, tentStr) == 0) {
    c = 't';
    WF = 1;
  }
    
  else if (strcmp(filter, bellStr) == 0) {
    c = 'l';
    WF = 3/2;
  }
    
  else if (strcmp(filter, mitchellStr) == 0) {
    c = 'm';
    WF = 2;
  }
    
  size_t width = pnm_get_width(ims);
  size_t height = pnm_get_height(ims);

  /* Construction de l'image de destination. */
  size_t newWidth = width;
  size_t newHeight = height;
  pnm imd = pnm_new(newWidth, newHeight, PnmRawPpm);

  /* Interpolation. */
  for (unsigned int i = 0; i<height; i++) {
    for (unsigned int jp = 0; jp<newWidth-1; jp++) {
      double j = (double)jp / (double)factor;
      
      unsigned int left = j - WF;
      
      unsigned int right = j + WF;
      if (right > newWidth-1)
        right = newWidth - 1;
      
      unsigned short s[3] = {0,0,0};
      
      for (unsigned int k = left; k<=right; k++)
        for (unsigned int channel = 0; channel<3; channel++) {
            switch (c) {
              case 'b':
                s[channel] += pnm_get_component(ims, i, k, channel) * box(k-j);
                break;
                
              case 't':
                s[channel] += pnm_get_component(ims, i, k, channel) * tent(k-j);
                break;
                
              case 'l':
                s[channel] += pnm_get_component(ims, i, k, channel) * bell(k-j);
                break;
                
              case 'm':
                s[channel] += pnm_get_component(ims, i, k, channel) * mitchell(k-j);
                break;
                
              default:
                s[channel] += pnm_get_component(ims, i, k, channel) * box(k-j);
                break;
            }
          }
      
      for (unsigned int channel = 0; channel<3; channel++)
        pnm_set_component(imd, i, jp, channel, s[channel]);
    }
  }
  
  return imd;
}

#define PARAM 4
int 
main(int argc, char *argv[])
{
  /* Verification des arguments. */
  if (argc != PARAM+1) usage(argv[0]);

  /* Lecture du facteur de magnification. */
  int factor = atoi(argv[1]);

  /* Chargement de l'image source. */
  pnm ims  = pnm_load(argv[3]);
  
  /* Magnification. */
  pnm imd = interpolation(ims, factor, argv[2]);
  imd = interpolationSansAgrand(transpose(imd), factor, argv[2]);
  imd = transpose(imd);
  
  /* Enregistrer l'image de destination. */
  pnm_save(imd, PnmRawPpm, argv[4]);

  pnm_free(ims);
  pnm_free(imd);

  return EXIT_SUCCESS;
}
