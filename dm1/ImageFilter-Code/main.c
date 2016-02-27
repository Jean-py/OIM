//
//  main.c
//  devoir1-OIM-2015

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#include "image.h"
#include "meanfilter.h"
#include "nonlocalmeanfilter.h"
#include "imagemetric.h"
#include "timer.h"

typedef struct{
	int operation;
	char inFilename[256];
	char outFilename[256];
	char diffFilename[256];
	int filtersize;
	int texturesize;
} s_args;

void parseArgs(char *argv[], s_args* args){
	args->operation = atoi(argv[1]);
	strncpy(args->inFilename, argv[2], 256);
	strncpy(args->outFilename, argv[3], 256);
	switch (args->operation) {
		case 0:
			args->filtersize = atoi(argv[4]);
			break;
		case 1:
			args->texturesize = atoi(argv[4]);
			args->filtersize = args->texturesize*2-1;
			break;
		case 2:
			args->filtersize = atoi(argv[4]);
			break;
		default:
			printf("Unknown operation -- abort\n");
			exit(args->operation);
	}
}

void usage(char * progname) {
	printf("usage : %s mode in out filtersize \n", progname);
	printf("mode :\n\t 0 : mean filter,\n\t 1 non local mean filter,\n\t 2 : distance between pictures\n");
	printf("in : input filename, pgm format\n");
	printf("out : output filename,pgm format\n");
	printf("filtersize : filter size, in pixels (filter is a square)\n");
	printf("roisize : region of interest size (only for non local mean filter)\n");
	printf("OR \nusage : %s 2 in out filtersize \n", progname);
	printf("mode :\n\t compute distance between pictures\n");
	printf("in : reference image filename, pgm format\n");
	printf("out : comparison image filename, pgm format\n");
	printf("filtersize : size (in pixels), of the comparison area (area is a square)\n");
	exit(EXIT_FAILURE);
	
}


int main(int argc, char *argv[]){
	s_args args;
	image* in = malloc(sizeof(image));
	image *out=malloc(sizeof(image));
	Timer* timer = malloc(sizeof(Timer));
	Time chrono =0.0;
	float resultat=0.0;



	if(argc<4){
		usage(argv[0]);
	}
	else{
		parseArgs(argv, &args);
	}
	

	switch(args.operation) {
		case  0 :
			if(args.filtersize < 0   || args.filtersize %2 == 0 ){
				fprintf(stderr,"Error dimension of filter \n");
				return -1;
			}
			image_readPgm(args.inFilename, in);
			image_readPgm(args.inFilename, out);
			
			timer_reset(timer);
			timer_start(timer);
			meanfilter(in,out,args.filtersize);
			timer_stop(timer);
			chrono = timer_value(timer);
			printf("localmean : %s %d %lf \n" , args.inFilename,args.texturesize,chrono);
			image_writePgm(args.outFilename ,out) ;
			image_release(in);
			image_release(out);
			break;
		
		case 1 :
			if(args.filtersize < 0   || args.filtersize %2 == 0 ){
					fprintf(stderr,"Error dimension of filter \n");
					return -1;
				}
				image_readPgm(args.inFilename, in);
				image_readPgm(args.inFilename, out);
				
				timer_reset(timer);
				timer_start(timer);
				nonlocal_meanfilter(in,out,args.filtersize,args.texturesize);
				timer_stop(timer);
				chrono = timer_value(timer);
				printf("nonlocalmean: %s %d %lf\n" , args.inFilename,args.texturesize,chrono);
				image_writePgm(args.outFilename ,out) ;
				image_release(in);
				image_release(out);
				break;

		case 2 :
			if(args.filtersize < 0   || args.filtersize %2 == 0 ){
					fprintf(stderr,"Error dimension of filter \n");
					return -1;
				}

				image_readPgm(args.outFilename, out);
				image_readPgm(args.inFilename, in);
				
				if ( (in->w != out->w) || (in->h != out->h) )
				{
					fprintf(stderr, "Error image don't have the same dimension\n" );
					return -1;
				}



				timer_reset(timer);
				timer_start(timer);
				resultat=compare_images(in,out,args.filtersize);
				timer_stop(timer);
				chrono = timer_value(timer);
				printf(" %s  %s %d %lf %f \n" , args.inFilename,args.outFilename,args.filtersize,chrono ,resultat);
				image_release(in);
				image_release(out);
				break;
		
		default :
			usage(argv[0]);
	}
	
	return 0;
}
