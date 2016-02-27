#include "imagemetric.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "utils.h"

float compare_images(image *ref, image *comp, int filtersize){

	int margin = filtersize/2;
	float resulInter=0.0;
	float result=0.0;

	pixel_t* pixel1 ;
	pixel_t* pixel2 ;
	int i,j;
	int count=0;

	for( i = 0 ; i < ref->w ; i++ ){
		for( j = 0 ; j < ref->h  ; j++ ){
			for (int k =i-margin; k <= i+margin; k++)
			{					
				for (int l = j-margin; l <= j+margin; l++)
				{
									
					if( k >= 0 && k < ref->w && l >= 0 && l < ref->h  ){
						resulInter =0;	
						pixel1 = image_pixelPtr(ref,k,l);
						pixel2 = image_pixelPtr(comp,k,l);	
						resulInter = (((float)(*pixel1)-(float)(*pixel2)) ) ;
						resulInter = resulInter/ 255.0;
						resulInter = resulInter *resulInter;						
						count++;
					}
					result += resulInter;
				}	
			}	
		}

	}
	return result/count;
}
