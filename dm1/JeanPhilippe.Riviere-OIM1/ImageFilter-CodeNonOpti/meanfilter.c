#include "meanfilter.h"
#include <string.h>
#include <stdio.h>

#include "utils.h"



void meanfilter(image *img, image *out, int filtersize){
	int i,j;
	int sommePartiel = 0 ;
	float cpt=0.0;
	int margin = (filtersize/2);
	
	for( i = 0 ; i < img->w ; i++ ){
		for( j = 0 ; j < img->h  ; j++ ){
			sommePartiel = 0;
			cpt=0.0;
			for (int k =  i-margin ; k <= i+margin; k++)
			{
				for (int l = j-margin ; l <= j+margin; l++)
				{
					if( k >= 0 && k < img->w && l >= 0 && l < img->h){  
						sommePartiel += image_pixel(img,  k,  l);
						cpt++;
					}					
				}	
			}
			*(image_pixelPtr(out, i,j )) = sommePartiel / cpt;
		}
	}
}