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

//optimisation
/*

#include "meanfilter.h"
#include <string.h>
#include <stdio.h>

#include "utils.h"



void meanfilter(image *img, image *out, int filtersize){
	int i,j;
	int sommePartiel = 0 ;
	int filtersizeCube = filtersize*filtersize;
	int margin = (filtersize/2);
	//int sumFirstLigne = 0;
	int sumFirstColumn = 0;
	int sumLastColumn =0;
	
	int firstColumn;

	for( i = 0 ; i < img->h ; i++ ){
		sommePartiel = 0;
		sumFirstColumn =0;
		sumLastColumn =0;
		for( j = 0 ; j < img->w  ; j++ ){
				for (int k = 0; k < filtersize; k++)
				{
					for (int l = 0; l < filtersize; l++)
					{

						//cas des bords de l'image
						if( k >= 0 && k < img->w && l >= 0 && l < img->h  ){  
							//cas de changement de ligne
							
							if( i-margin < 0 || i+margin+1>img->w || j-margin<0 || j+margin+1>img->h){
								sommePartiel += image_pixel(img,  k-margin,  l-margin);
							}
							else if ((i-margin == 0 && j-margin == 0) || (i+margin+1==img->w &&  j+margin+1==img->h) ){
								sommePartiel += image_pixel(img,  k-margin,  l-margin);
							}
							else{
								if(l  == 0)
								{
									sumFirstColumn += image_pixel(img,  k-margin,  l-margin);  
								}
								else if(l == filtersize){
									sumLastColumn += image_pixel(img,  k-margin,  l-margin) ;
								} 
							}
						}						
					}	
				}
				

				if(j != 0 ){
					sommePartiel -=  sumFirstColumn ;
					sommePartiel += sumLastColumn;
				}
				sumFirstColumn =0;
				
				//printf("sommePartiel : %d  et sommePartiel / filtersizeCube = %d , i %d, j %d \n",sommePartiel ,sommePartiel / filtersizeCube,i,j);
				*(image_pixelPtr(out, i,j )) = sommePartiel / filtersizeCube;
				if( i-margin < 0 || i+margin+1>img->w || j-margin<0 || j+margin+1>img->h){
					sommePartiel =0;
				}
		
		}
	}
}
*/
