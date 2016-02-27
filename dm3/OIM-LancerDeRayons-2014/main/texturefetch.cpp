#include "texturefetch.hpp"
#include "raytracing.h"

#include <iostream>

/*
 *  Pour la documentation de ces fonctions, se rapporter à la page "Textures" de la documentation
 */
/*
 * @brief getTexel Get a single texel from a texture.
 * This function access the texture at coordinates (row, column) and fetch the value of the corresponding texel (pixel from a texture)
 * @param pixels    The image to access, organized as a linear array of texel arranged by row
 * @param width     Width of the image
 * @param height    Height of the image
 * @param depth     Depth of the image (number of component by texel)
 * @param row       Row coordinate of the requested texel
 * @param column    Column coordinate of the requested texel
 * @return          The value of the texel
 * @todo            Transfom the 2D coordinates in a 1D index and get the corresponding texel value
 */
Color getTexel(unsigned char *pixels, int width, int height, int depth, int column, int row){
  Color color(1.0, 1.0, 1.0);
  int index =  row*width*depth + column*depth;
  float r,v,b,a;

  if(depth == 1){
    color = (float) pixels[index]/255.f;
  } else if (depth == 3 || depth == 4){
    r = (float)pixels[index]/255.f;
    v = (float)pixels[index+1]/255.f;
    b = (float)pixels[index+2]/255.f;
    color = Color(r,v,b);
  } 
  if (depth == 4) {
   a = (float)pixels[index+3]/255.f;
   color = color * a;
  }
  return color;
}



/*
 * @brief interpolateTexture Get a texel linearly interpolated from its neighbors
 * @param pixels    The image to access, organized as a linear array of texel arranged by row
 * @param width     Width of the image
 * @param height    Height of the image
 * @param depth     Depth of the image (number of component by texel)
 * @param s         The column coordinate of the requested texel as a floating point
 * @param t         The row coordinate of the requested texel as a floating point
 * @return          The value of the interpolated texel
 * @todo            Devoir 3, 2014 : From the floating point coordinates, compute the integer part and the fractional part. The integer part is the used to acces the 4 texels implied in the interpolation (One texel and its 3 up and right neighors), the fractional part is used to linearly interpolate from neighbors.
 */
 Color interpolateTexture(unsigned char *pixels, int width, int height, int depth, float s, float t){

  int sPrim = (int)s;
  int tPrim = (int)t;   
  float ds  =  t-std::floor(t);
  float dt  =   t-std::floor(t);
  Color R1,R2,Q11 , Q12 , Q21 , Q22,color;
  Q11 = getTexel(pixels , width,height,depth,sPrim,tPrim);
  Q12 = getTexel(pixels , width,height,depth,sPrim,tPrim+1);
  Q21 = getTexel(pixels , width,height,depth,sPrim+1,tPrim);
  Q22 = getTexel(pixels , width,height,depth,sPrim+1,tPrim+1);

  R1 = Q11*(1-ds) + Q21*ds;
  R2 = Q12*(1-ds) + Q22*ds;
  color = R1*(1-dt) + R2*dt;
  return color;

 }


/*
 * @brief integrateTexture Get a texel by computing the mean of the color on a neighborood of size (deltas x deltat)
 * @param pixels    The image to access, organized as a linear array of texel arranged by row
 * @param width     Width of the image
 * @param height    Height of the image
 * @param depth     Depth of the image (number of component by texel)
 * @param s         The column coordinate of the requested texel as a floating point
 * @param t         The row coordinate of the requested texel as a floating point
 * @param deltas    The size, in the column dimension, of the neighborood
 * @param deltat    The size, in the row dimension, of the neighborood
 * @return
 * @todo
 */
 Color integrateTexture(unsigned char *pixels, int width, int height, int depth, float s, float t, int deltas, int deltat){

  int sPrim = (int)s;
  int tPrim = (int)t;
  Color color(0.0f,0.0f,0.0f);
  for(int i = sPrim; i < sPrim + deltas; ++i){
    for(int j = tPrim; j < tPrim + deltat; ++j){
     color = color +  getTexel(pixels, width, height, depth, i, j) ;
    }
  }
  color = color * (1.0f/(deltas * deltat));
  return color;
}

/*
 * @brief prefilterTexture Compute an array of images with geometrically decreasing resolution from the original image.
 * @param imagearray The array of images to compute. element at index 0 in this array is the full resolution image and must not be modified
 * @param width     Width of the full resolution image
 * @param height    Height of the full resolution image
 * @param depth     Depth of the image (number of component by texel)
 * @param nblevels  Number of level to compute : nblevels = log2(min(width, height))
 * @return          if the array may be filled, return true, else return false
 */
 bool prefilterTexture(unsigned char **imagearray, int width, int height, int depth, int nblevels){

  Color color;
  int index;
  int w = width;
  int h = height;
  int level = 1 ,C = 0,R =0;
  for( level = 1 ; level <nblevels ; ++level)
  {	
    w = w/2;
    h = h/2;
    imagearray[level] = (unsigned char *)malloc(w*h*depth * sizeof(unsigned char));
    
    for(C =0 ;C < w ;++C )
    {
      for(R = 0; R < h; ++R)
      {	     	
        index =   (R * w * depth) + (C * depth) ;
        color = Color( 0.0f, 0.0f, 0.0f );

        for (int i = 2*C; i <= 2*C+1; ++i) {
          for (int j = 2*R; j <= 2*R+1; ++j){
            color = color + getTexel(imagearray[level-1], w*2, h*2, depth, i, j);
          }
        }
        color = color * 0.25f;				
        imagearray[level][index] = color[0]*255;
        imagearray[level][index + 1] = color[1]*255;
        imagearray[level][index + 2] = color[2]*255;
      }
    }  
  }
  return (level == nblevels && C==w && R==h )?true	: false;
}  

