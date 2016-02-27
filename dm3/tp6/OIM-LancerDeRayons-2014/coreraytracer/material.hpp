#pragma once

#include "bsdf.hpp"
#include "diff_geom.hpp"
#include "texture.hpp"

/** @ingroup Raytracer 
 *  Abstract class for material
 */
class Material {
protected:
    Texture *mTexture;

public:
    Material() : mTexture(NULL){}

    virtual BSDF evaluate (const Diff_Geom& diff_geom_, float *lod) const =0;

    void setTexture(Texture *tex){
        mTexture=tex;
    }

    bool hasTexture() const {return mTexture!=NULL;}

protected:
    Color getTextureColor(float u, float v, float lod) const{
        Color c;
        mTexture->getPixel(u, v, lod, c);
        return c;
    }
};
