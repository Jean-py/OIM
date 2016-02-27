#include "image.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

/**
 *	Initialize an image of size w x h
 */
void image_init(image *img, int w, int h){
	img->w=w;
	img->h=h;
	img->data=malloc(sizeof(pixel_t)*img->w*img->h);
	memset(img->data, 0, img->w*img->h*sizeof(pixel_t));
}

/**
 *	Release the memory used to store an image
 */
void image_release(image *img){
	free(img->data);
}

// from http://ugurkoltuk.wordpress.com/2010/03/04/an-extreme-simple-pgm-io-api/
void skipComments(FILE *fp)
{
	int ch;
	char line[100];
 
	while ((ch = fgetc(fp)) != EOF && isspace(ch))  ;
	
	if (ch == '#') {
		fgets(line, sizeof(line), fp);
		skipComments(fp);
	} else
		fseek(fp, -1, SEEK_CUR);
}

/**
 * Construct an image from a .pgm file
 * IMPORTANT : this function PERFORMS img->data malloc
 */
void image_readPgm(const char *filename, image *img){
	FILE *pgmFile;
	char version[3];
	int i, j;
	int lo;
	
	pgmFile = fopen(filename, "rb");
	if (pgmFile == NULL) {
		perror("cannot open file to read");
		fprintf(stderr, "%s\n", filename);
		exit(EXIT_FAILURE);
	}
	
	fgets(version, sizeof(version), pgmFile);
	if (strcmp(version, "P5")) {
		fprintf(stderr, "Wrong file type!\n");
		exit(EXIT_FAILURE);
	}
	
	skipComments(pgmFile);
	fscanf(pgmFile, "%d", &img->w);
	skipComments(pgmFile);
	fscanf(pgmFile, "%d", &img->h);
	skipComments(pgmFile);
	int max_value;
	fscanf(pgmFile, "%d", &max_value);
	// skip CR
	fgetc(pgmFile);
	assert(max_value == 255);
	
	img->data = malloc(sizeof(pixel_t)*img->w*img->h);
	for (i = 0; i < img->h; ++i)
		for (j = 0; j < img->w; ++j) {
			lo = fgetc(pgmFile);
			img->data[i*img->w+j] = (pixel_t)lo;
		}
	fclose(pgmFile);
	return;
}

/**
 * Write an image to a .pgm file
 * IMPORTANT : this function DO NOT free img->data (in case of reuse by the caller)
 */
void image_writePgm(const char *filename, const image *img) {
	FILE *pgmFile;
	int i, j;
	int lo;
 
	pgmFile = fopen(filename, "wb");
	if (pgmFile == NULL) {
		perror("cannot open file to write");
		fprintf(stderr, "%s\n", filename);
		exit(EXIT_FAILURE);
	}
 
	fprintf(pgmFile, "P5 ");
	fprintf(pgmFile, "%d %d ", img->w, img->h);
	fprintf(pgmFile, "%d ", 255);
	
	for (i = 0; i < img->h; ++i)
		for (j = 0; j < img->w; ++j) {
			lo = img->data[i*img->w+j];
			fputc(lo, pgmFile);
		}
	fclose(pgmFile);
}
// end of code from http://ugurkoltuk.wordpress.com/2010/03/04/an-extreme-simple-pgm-io-api/

/**
 *	Accessor to the pixel at coordinate (x,y)
 *		expect (0 <= x <img->w) AND (0 <= h <img->y)
 *		return	the value of the pixel img(x,y)
 */
pixel_t image_pixel(image * img, int x, int y){
	return img->data[y*img->w +x];
}

/**
 *	Accessor to the pixel adress at coordinate (x,y).
 *	This function is usefull to modify a pixel value in the image
 *		expect (0 <= x <img->w) AND (0 <= h <img->y)
 *		return	the adress of the pixel img(x,y)
 */
pixel_t * image_pixelPtr(image * img, int x, int y){
	return &(img->data[y*img->w +x]);
}

