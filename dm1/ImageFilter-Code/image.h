#ifndef IMAGE_H
#define IMAGE_H

/**
 *	Abtract data type for managing images with external representation as gray scale pgm files
 *		(see specification of this format at http://netpbm.sourceforge.net/doc/pgm.html )
 *
 */

/**
 * pixel are stored as unsigned char
 */
typedef unsigned char pixel_t;

/**
 * An image is just a one dimensional array of pixel with its dimension : width and heigth
 */
typedef struct {
	int w;
	int h;
	pixel_t *data;
} image;

/**
 *	Initialize an image of size w x h
 */
void image_init(image *img, int w, int h);
/**
 *	Release the memory used to store an image
 */
void image_release(image *img);

/**
 * Construct an image from a .pgm file
 * IMPORTANT : this function PERFORMS img->data malloc
 */
void image_readPgm(const char *filename, image *img);

/**
 * Write an image to a .pgm file
 * IMPORTANT : this function DO NOT free img->data (in case of reuse by the caller)
 */
void image_writePgm(const char *filename, const image *img);


/**
 *	Accessor to the pixel at coordinate (x,y)
 *		expect (0 <= x <img->w) AND (0 <= h <img->y)
 *		return	the value of the pixel img(x,y)
 */
pixel_t image_pixel(image * img, int x, int y);

/**
 *	Accessor to the pixel adress at coordinate (x,y). 
 *	This function is usefull to modify a pixel value in the image
 *		expect (0 <= x <img->w) AND (0 <= h <img->y)
 *		return	the adress of the pixel img(x,y)
 */
pixel_t * image_pixelPtr(image * img, int x, int y);



#endif

