#include "nonlocalmeanfilter.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"


void nonlocal_meanfilter(image *img, image *out, int filtersize, int texturesize){
	
	float moy=0;
		for (int  i = 0; i < img->w ; i++)
		{
			
			for ( int j = 0; j < img-> h; j++)
			{
				moy = moyPixelR(img,i,j,filtersize,texturesize ) ;
				*(image_pixelPtr(out,i,j)) = moy ;
			}	
		}
	return;
}


float distanceEuclidienne(int i,int j,int k , int l,int texturesize , image* img){
	if (texturesize%2 ==0)
	{
		perror("Erreur texture need to be odd\n");
		return -1;
	}

	int margin = texturesize/2;
	int m =0;
	int n=0;
	float result=0.0;
	pixel_t* pixel1 ;
	pixel_t* pixel2 ;

	pixel1 = image_pixelPtr(img,i-margin+m,j-margin+n);
	pixel2 = image_pixelPtr(img,k-margin+m,l-margin+n);	

		for ( m = 0 ; m <= texturesize  ; m++)
		{
		
					
			for ( n = 0 ; n <= texturesize; n++)
			{		
			
				result+= (((float)(*pixel1)-(float)(*pixel2)) ) ;
				pow(result,2);
				result /= 255.0;
				pixel1 = image_pixelPtr(img,i-margin+m,j-margin+n);
				pixel2 = image_pixelPtr(img,k-margin+m,l-margin+n);	
				
			}
		}
	return result;
}



float moyPixelR (image*  img ,int i,int j  ,int filtersize ,int texturesize ){

	
	float result =0.0;
	float cpt =0;
	float disEuclid;
	int middleBlocTexture = texturesize/2;
	int middleBlocFilter = filtersize/2;
	float expo =0.0;

	pixel_t* pixel1;
	

	for (int k = middleBlocTexture ; k < filtersize - middleBlocTexture;k++)
	{
		for (int l= middleBlocTexture; l <filtersize - middleBlocTexture; l++)
		{		
			pixel1 = image_pixelPtr(img,i- middleBlocFilter + k,j - middleBlocFilter + l);	
			if(i- middleBlocFilter + k >= 0 && i- middleBlocFilter + k < img-> w && j - middleBlocFilter + l >= 0 && j - middleBlocFilter + l < img->h) {
				disEuclid =  distanceEuclidienne(i,j, i-middleBlocFilter+k , j- middleBlocFilter+l ,texturesize,img)   ;
				expo = exp(-1* disEuclid);
				result += expo  * (float)(*pixel1);
				cpt +=  expo;
			}
		}
	}
	
	return result/cpt;



}








