#include "image.h"

#ifndef __NLMEANFILTER__
#define __NLMEANFILTER__

void nonlocal_meanfilter(image *img, image *out, int filtersize, int texturesize);
float distanceEuclidienne(int i,int j,int k , int l, int filtersize, image* img);
float moyPixelR (image*  Roi ,int i,int j  ,int filtersize ,int texturesize );

#endif
