#pragma once

#include "color.hpp"
#include "vector3d.hpp"

#include <limits>
#include <string>


/** @ingroup Raytracer
 *  Texture definition
 */
/*
#define TEX_LUMINANCE 0
#define TEX_LUMINANCE_ALPHA 1
#define TEX_RGB 2
#define TEX_RGBA 3
*/

class Texture {
private:
    unsigned char *mPixels;
    unsigned char **mMipMapLevels;
    bool hasmipmap;
    int mPixelFormat;
    int mWidth;
    int mHeight;
    int mlodmax;
    float mSu;
    float mSv;

    bool clamp;
    int load(std::string filename);

    Color integrateMipMap(int level, float s, float t, int w, int h);
    Color integrateExplicit(unsigned char *pixels, int imageWidth, int imageHeight, int pixelFormat, float s, float t, int w, int h);

    Color interpolate(unsigned char *pixels, int w, int h, int s, int t, float ds, float dt);

    bool buildmipmaps();

public:
    Texture(std::string filename);
    ~Texture();

    bool prefilter();

    void setScale(float su, float sv);


    // u and v between 0 and 1
    void getPixel(float u, float v, float lod, Color &color);

    float computeMipmapLevel(const Vector3D &dTdx, const Vector3D &dTdy) {
        return std::log2(std::max(dTdx.norm()*width()*mSu, dTdy.norm()*height()*mSv));
    }

    int width() const{
        return mWidth;
    }

    int height() const {
        return mHeight;
    }
	
    bool getclamp() const;
    void setclamp(bool value);
};

