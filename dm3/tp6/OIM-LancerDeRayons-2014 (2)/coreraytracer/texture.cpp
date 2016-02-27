#include "texture.hpp"
#include "texturefetch.hpp"

#include <png.h>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <assert.h>

Texture::~Texture(){
    delete[] mPixels;
    for (int i=1; i<mlodmax+1; ++i)
        delete[] mMipMapLevels[i];
    delete[] mMipMapLevels;
}


bool Texture::getclamp() const
{
    return clamp;
}

void Texture::setclamp(bool value)
{
    clamp = value;
}

int Texture::load(std::string filename){
    FILE *fp;
    png_byte magic[8];

    /* open image file */
    fp = std::fopen (filename.c_str(), "rb");
    if (!fp) {
        fprintf (stderr, "error: couldn't open \"%s\"!\n", filename.c_str());
        return 0;
    }

    /* read magic number */
    fread (magic, 1, sizeof (magic), fp);

    /* check for valid magic number */
    if (!png_check_sig (magic, sizeof (magic))) {
        fprintf (stderr, "error: \"%s\" is not a valid PNG image!\n", filename.c_str());
        fclose (fp);
        return 0;
    }

    /* create a png read struct */
    png_structp png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose (fp);
        return 0;
    }

    /* create a png info struct */
    png_infop info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr) {
        fclose (fp);
        png_destroy_read_struct (&png_ptr, NULL, NULL);
        return 0;
    }

    /* initialize the setjmp for returning properly after a libpng
       error occured */
    if (setjmp (png_jmpbuf (png_ptr))) {
        fclose (fp);
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        return 0;
    }
    /* setup libpng for using standard C fread() function
       with our FILE pointer */
    png_init_io (png_ptr, fp);

    /* tell libpng that we have already read the magic number */
    png_set_sig_bytes (png_ptr, sizeof (magic));

    /* read png info */
    png_read_info (png_ptr, info_ptr);

    int bit_depth, color_type;

    /* get some usefull information from header */
    bit_depth = png_get_bit_depth (png_ptr, info_ptr);
    color_type = png_get_color_type (png_ptr, info_ptr);
    /* convert index color images to RGB images */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb (png_ptr);
    /* convert 1-2-4 bits grayscale images to 8 bits
       grayscale. */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha (png_ptr);
    if (bit_depth == 16)
      png_set_strip_16 (png_ptr);
    else if (bit_depth < 8)
      png_set_packing (png_ptr);
    /* update info structure to apply transformations */
    png_read_update_info (png_ptr, info_ptr);

    /* retrieve updated information */
    png_get_IHDR (png_ptr, info_ptr,
                  (png_uint_32*)(&mWidth),
                  (png_uint_32*)(&mHeight),
                  &bit_depth, &color_type,
                  NULL, NULL, NULL);
    switch (color_type) {
      case PNG_COLOR_TYPE_GRAY:
        mPixelFormat = 1;
        break;

      case PNG_COLOR_TYPE_GRAY_ALPHA:
        mPixelFormat = 2;
        break;

      case PNG_COLOR_TYPE_RGB:
        mPixelFormat = 3;
        break;

      case PNG_COLOR_TYPE_RGB_ALPHA:
        mPixelFormat = 4;
        break;

      default:
        /* Badness */
        break;
      }

    /* we can now allocate memory for storing pixel data */
    mPixels = new unsigned char[mWidth * mHeight * mPixelFormat];

    png_bytep *row_pointers;

    /* setup a pointer array.  Each one points at the begening of a row. */
    row_pointers = new png_bytep[mHeight];

    for (int i = 0; i < mHeight; ++i) {
        row_pointers[i] = (png_bytep)(mPixels +
          ((mHeight - (i + 1)) * mWidth * mPixelFormat));
    }

    /* read pixel data using row pointers */
    png_read_image (png_ptr, row_pointers);

    /* finish decompression and release memory */
    png_read_end (png_ptr, NULL);
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

    fclose (fp);

    /* we don't need row pointers anymore */
    delete[] row_pointers;
    return 1;
}

Texture::Texture(std::string filename) : mPixels(NULL), mMipMapLevels(NULL), hasmipmap(false), mSu(1.f), mSv(1.f), clamp(false){
    if (!load(filename)) {
        std::cerr << "Erreur : impossible de lire le fichier " << filename << std::endl;
    } else {
        mlodmax = (int)std::floor(std::min(std::log2(mWidth), std::log2(mHeight)));
    }
}

bool Texture::prefilter(){
    hasmipmap = buildmipmaps();
    return hasmipmap;
}

bool Texture::buildmipmaps(){
	mMipMapLevels = new unsigned char*[mlodmax+1];
	mMipMapLevels[0] = mPixels;
    for (int i=1;i<mlodmax+1; ++i)
        mMipMapLevels[i]=NULL;

    // Call of the student function
    return prefilterTexture(mMipMapLevels, mWidth, mHeight, mPixelFormat, mlodmax+1);

}

void Texture::setScale(float su, float sv){
	mSu = 1.f/su;
	mSv = 1.f/sv;
}


void Texture::getPixel(float u, float v, float lod, Color &color){

        if (clamp){
            u = (u>0) ? ((u<1) ? u : 1) : 0;
            v = (v>0) ? ((v<1) ? v : 1) : 0;
        }

        u = mSu*u;
        v = mSv*v;

        float scaledU = u * (mWidth - 1);
        float scaledV = v * (mHeight - 1);
//        color = Color(u,v,0);
//        return;

        int s = ((int)scaledU) % mWidth;
        int t = ((int)scaledV) % mHeight;


        float ds = scaledU - std::floor(scaledU);
        float dt = scaledV - std::floor(scaledV);

		if (s<0){
			s = s+mWidth;
		}
		if (t<0){
			t = t+mHeight;
		}

        // Nearest
//        color = interpolate(mPixels, mWidth, mHeight, s,t, 0.f,0.f);
//        return;

         // Linear
//         color = interpolate(mPixels, mWidth, mHeight, s,t, ds, dt);
//         return;

        /* Filtrage */
        lod = std::min(lod-1, (float)mlodmax);
        if (lod > 0) {
//            color = Color(0, std::floor(lod), 0);
//            return;

            int baselod = (int)std::floor(lod);
            int pix = (int)(std::pow(2.f,baselod)); // bigger inferior mipmap level

            if (hasmipmap){
                // Sous-echantillonnage --> filtrage mip-map
                Color color1 = integrateMipMap(baselod, ((float)s + ds)/pix, ((float)t + dt)/pix, mWidth/pix, mHeight/pix);

                if (baselod < mlodmax){
                    pix = std::pow(2.f,baselod+1); // smaller superior mipmap level
                    Color color2 = integrateMipMap(baselod+1, ((float)s + ds)/pix, ((float)t + dt)/pix, mWidth/pix, mHeight/pix);

                    float dm = lod - std::floor(lod);
                    color = color1 * (1-dm) + color2 * dm;
                } else {
                    color = color1;
                }
            } else {
                int cs = pix * (s/pix);
                int ct = pix * (t/pix);
                Color color1 = integrateExplicit(mMipMapLevels[0], mWidth, mHeight, mPixelFormat, cs, ct, pix, pix);

                if (baselod < mlodmax){
                    pix = std::pow(2.f,baselod+1); // smaller superior mipmap level
                    cs = pix * (s/pix);
                    ct = pix * (t/pix);
                    Color color2 = integrateExplicit(mMipMapLevels[0], mWidth, mHeight, mPixelFormat, cs, ct, pix, pix);

                    float dm = lod - std::floor(lod);
                    color = color1 * (1-dm) + color2 * dm;
                } else {
                    color = color1;
                }

            }
        } else {
//            color = Color(1/*fabsf(lod)*/, 0, 0);
//            return;

            color = interpolate(mPixels, mWidth, mHeight, s,t, ds,dt);
        }

}

Color Texture::integrateMipMap(int level, float s, float t, int w, int h){
    Color color(0.f, 0.f, 0.f);
    color = interpolate(mMipMapLevels[level], w, h, (int)s, (int)t, s-std::floor(s), t-std::floor(t));
    return color;
}

Color Texture::integrateExplicit(unsigned char *pixels, int imageWidth, int imageHeight, int pixelFormat, float s, float t, int w, int h){

#if 0
    Color color(0.f, 0.f, 0.f);
    for (int i=0; i<h; i++)
        for (int j=0; j<w; j++) {
            int k = ((int)(t+i)) % imageHeight;
            int l = ((int)(s+j)) % imageWidth;
            int linPos = (k*imageWidth + l)*(pixelFormat);
            Color centerColor(((float) pixels[linPos])/255.0, ((float) pixels[linPos+1])/255.0, ((float) pixels[linPos+2])/255.0);
            color = color + centerColor;
        }
    color = color * (1.f/(w*h));
    return color;
#endif
    // Call of the student function
    return integrateTexture(pixels, imageWidth, imageHeight, pixelFormat, s, t, w, h);
}

Color Texture::interpolate(unsigned char *pixels, int w, int h, int s, int t, float ds, float dt){

#if 0
    Color color;
    // Sur échantillonnage --> interpolation
    // nearest texel
    int linPos = (t*w + s)*(mPixelFormat);
    Color centerColor(((float) pixels[linPos])/255.0, ((float) pixels[linPos+1])/255.0, ((float) pixels[linPos+2])/255.0);

    Color rightColor;
    int r = std::min(s+1, w-1);
    int posds = (t*w + r)*(mPixelFormat);
    rightColor = Color(
                      ((float) pixels[posds])/255.0,
                      ((float) pixels[posds+1])/255.0,
                      ((float) pixels[posds+2])/255.0
                  );
    Color upColor;
    int p = std::min(t+1, h-1);
    int posdt = (p*w + s)*(mPixelFormat);
    upColor = Color (
                    ((float) pixels[posdt])/255.0,
                    ((float) pixels[posdt+1])/255.0,
                    ((float) pixels[posdt+2])/255.0
              );
    Color upRightColor;
    int posdtds = (p*w + r)*(mPixelFormat);
    upRightColor = Color (
                         ((float) pixels[posdtds])/255.0,
                         ((float) pixels[posdtds+1])/255.0,
                         ((float) pixels[posdtds+2])/255.0
                   );
    rightColor = centerColor*(1-ds) + rightColor*ds;
    upColor = upColor*(1-ds) + upRightColor*ds;
    color = rightColor*(1-dt) + upColor*dt;
    return color;
#endif

    // Call of the student function
    return interpolateTexture(pixels, w, h, mPixelFormat, (float)s+ds, (float)t+dt);

}
