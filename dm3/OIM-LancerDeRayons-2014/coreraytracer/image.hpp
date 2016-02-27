#pragma once

#include "color.hpp"


#include <string>

#include <ImfOutputFile.h>
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <half.h>
#include <png.h>

using namespace std;
using namespace Imf;
using namespace Imath;


/** @ingroup Raytracer 
 *  Image management
 */

/// Gamma value for standard LCD monitors
#define GAMMA 2.2f
/// Default key value for global tone mapping
#define KEYVALUE 0.18f

class Image {
private:

	int m_x_res, m_y_res;

	Color * m_pixels;


public:

	Image () : m_x_res (0), m_y_res (0),
			   m_pixels (NULL) {
	}

	Image (int x_res_, int y_res_) : m_x_res (x_res_), m_y_res (y_res_) {
		m_pixels = new Color[x_res_ * y_res_];
	}

	~Image() {
		delete[] m_pixels;
	}

	const Color& pixel (int i_, int j_) const {
        return m_pixels[(m_y_res-1-j_) * m_x_res + i_];
	}
	
	Color& pixel (int i_, int j_) {
        return m_pixels[(m_y_res-1-j_) * m_x_res + i_];
	}

	int x_res () const {
		return m_x_res;
	}

	int y_res () const {
		return m_y_res;
	}

	void write_to_exr_file (const string& file_name_) {
		Header header (m_x_res, m_y_res);
		
		//edit the active zone.
		Box2i data_window (V2i (0, 0),
						   V2i (m_x_res - 1, m_y_res - 1));

		header.dataWindow() = data_window; //beuark.

		header.channels().insert ("R", Channel (HALF));
		header.channels().insert ("G", Channel (HALF));
		header.channels().insert ("B", Channel (HALF));	
	
		const int x_count = m_x_res;
		const int nb_pixels = m_x_res * m_y_res;
	
		half * half_rgb = new half[3 * nb_pixels];	
	
		int offset = 0;
		int num_pixel = 0;
	
		for (int y = 0; y < m_y_res; y++) {
			for (int x = 0; x < m_x_res; x++, num_pixel++) {
                Color color = pixel (x, m_y_res-1-y);
			
				for (int i = 0; i < 3; i++, offset++) {
					half_rgb[offset] = color[i];
				}			
			}
		}
	
		offset = 0;
	
		half_rgb -= 3 * offset;	

		FrameBuffer fb;
		//there are 3 * sizeof(half) bytes between two R elements.
		fb.insert ("R", Slice (HALF, (char *)half_rgb, 3 * sizeof (half),
							   3 * x_count * sizeof (half)));
		//the first element of G is sizeof(half) after the first element of R.
		fb.insert ("G", Slice (HALF, (char *)half_rgb + sizeof(half), 3 * sizeof (half),
							   3 * x_count * sizeof (half)));
		//the first B element is 2 * sizeof (half) bytes after the first element of R.
		fb.insert ("B", Slice (HALF, (char *)half_rgb + 2 * sizeof(half), 3 * sizeof (half),
							   3 * x_count * sizeof (half)));
		try {
			OutputFile file (file_name_.c_str(), header);
			file.setFrameBuffer (fb);
			//y_count() rows to write
			file.writePixels (m_y_res);
		} catch (const std::exception &e) {
			std::cerr<<"Unable to write image file "<<file_name_<<" : "<<e.what()<<std::endl;
		}

		//release the memory, but come back to the real address before.
		delete[] (half_rgb + 3 * offset);	
	}


    /* write a png file */
    int save_png_to_file (unsigned char *image, int width, int height, const char *file_name) {
       FILE *fp;
       png_structp png_ptr;
       png_infop info_ptr;
       /* open the file */
       fp = fopen(file_name, "wb");
       if (fp == NULL)
          return (0);
       png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
       if (png_ptr == NULL) {
          fclose(fp);
          return (0);
       }
       info_ptr = png_create_info_struct(png_ptr);
       if (info_ptr == NULL) {
          fclose(fp);
          png_destroy_write_struct(&png_ptr,  NULL);
          return (0);
       }
       if (setjmp(png_jmpbuf(png_ptr))) {
          /* If we get here, we had a problem writing the file */
          fclose(fp);
          png_destroy_write_struct(&png_ptr, &info_ptr);
          return (0);
       }
       png_init_io(png_ptr, fp);
       png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
          PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
       //png_set_gAMA(png_ptr, info_ptr, 1.0);
       png_write_info(png_ptr, info_ptr);
       png_bytep row_pointers[height];
       for (int k = 0; k < height; k++)
         row_pointers[k] = image + k*width*3;
       png_write_image(png_ptr, row_pointers);
       png_write_end(png_ptr, info_ptr);
       png_destroy_write_struct(&png_ptr, &info_ptr);
       fclose(fp);
       return (1);
    }

    void postProcess(const Color *pixels, unsigned char *rgb_chanels, int width, int height){
        double *xyY_chanels = new double [width*height*3];
        double keyValue = KEYVALUE;
        double delta = 1e-6f;
        double avLum = 0.0f;
        /* sRGB D65 matrix from http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html */
        double rgbToXyzMatrix[3][3] = {
            {0.4124564, 0.3575761,  0.1804375},
            {0.2126729,  0.7151522,  0.0721750},
            {0.0193339,  0.1191920,  0.9503041}

        };
        /* sRGB D65 matrix from http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html */
        double xyzToRgbMatrix[3][3] = {
            {3.2404542, -1.5371385, -0.4985314},
            {-0.9692660,  1.8760108,  0.0415560},
            {0.0556434, -0.2040259,  1.0572252}
        };


#define X xyz[0]
#define Y xyz[1]
#define Z xyz[2]
        int maxI = width*height;
        for (int i=0; i<maxI; ++i) {
            double w;
            // Transform from RGB to XYZ
            Color pixel = pixels[i];
            double xyz[3] = {0.0, 0.0, 0.0};
            for (int ii = 0; ii < 3; ++ii)
                for (int jj = 0; jj < 3; ++jj)
                    xyz[ii] += rgbToXyzMatrix[ii][jj] * (double)(pixel[jj]);


            // Transform from XYZ to xyY
            w = (X + Y + Z);
            if(w > 0.) {
                xyY_chanels[i*3    ] = X/w; // x
                xyY_chanels[i*3 + 1] = Y/w; // y
                xyY_chanels[i*3 + 2] = Y; // Y
                avLum += std::log(std::max(Y, 0.) + delta);
            } else {
                xyY_chanels[i*3    ] = 0.; // x
                xyY_chanels[i*3 + 1] = 0.; // y
                xyY_chanels[i*3 + 2] = 0.; // Y
            }
        }
        avLum = exp(avLum/(double)(width*height));

        maxI*=3;
        for(int i=0; i<maxI; i+=3) {
            double xyz[3] = {0.0, 0.0, 0.0};
            // tonemap
            double lum = std::max(xyY_chanels[i+2], 0.);
            lum = (keyValue*lum)/avLum;
            float lumWhite = avLum*100000.;
            xyY_chanels[i+2] = lum*(1.0+(lum/(lumWhite*lumWhite)))/(1.0+lum);//lum/(1.0+lum);

            // Transform from xyY to XYZ
            Y = xyY_chanels[i+2] ;

            //if((Y > 0.0) && (xyY_chanels[i] > 0.0) && (xyY_chanels[i+1] > 0.0)) {
            if((xyY_chanels[i] > 0.0) && (xyY_chanels[i+1] > 0.0)) {
                X = (Y * xyY_chanels[i]) / xyY_chanels[i+1];
                Z = (X / xyY_chanels[i]) - X - Y;
            }
            else {
                X = Z = 0.0;
            }

            // conversion XYZ vers RGB
            double rgb[3] = {0.0, 0.0, 0.0};
            for (int ii = 0; ii < 3; ++ii){
                for (int jj = 0; jj < 3; ++jj)
                    rgb[ii] += xyzToRgbMatrix[ii][jj] * xyz[jj];
                rgb[ii] = std::max(rgb[ii],0.0);
            }


            // gamma correction
            float gamma_corr = 1.f/2.4f;
            for (int k=0;k<3;k++)
                if (rgb[k] <= 0.0031308f)
                    rgb[k] = 12.92f*rgb[k];
                else
                    rgb[k] = (1.f + 0.055f) * std::pow(rgb[k],gamma_corr) - 0.055f;

            rgb_chanels[i    ] = (unsigned char)(std::min(rgb[0]*255.,255.));
            rgb_chanels[i + 1] = (unsigned char)(std::min(rgb[1]*255.,255.));
            rgb_chanels[i + 2] = (unsigned char)(std::min(rgb[2]*255.,255.));
        }
#undef X
#undef Y
#undef Z
        delete[] xyY_chanels;
    }

    void write_to_png_file(const char *filename){
        unsigned char *rgb_LDRpixels = new unsigned char[m_x_res*m_y_res * 3];
        postProcess(m_pixels, rgb_LDRpixels, m_x_res, m_y_res);
        save_png_to_file (rgb_LDRpixels, m_x_res, m_y_res, filename);
        delete[] rgb_LDRpixels;
    }
};
