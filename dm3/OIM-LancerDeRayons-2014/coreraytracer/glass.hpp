#pragma once

#include "material.hpp"

/** @ingroup Raytracer 
 *  Glass material
 */
class Glass : public Material {
private:

	Color m_kr, m_kt;
	float m_ior;
	
public:

    Glass (const Color& kr_, const Color& kt_, float ior_) : Material(), m_kr (kr_), m_kt (kt_), m_ior (ior_) {
	}

    BSDF evaluate (const Diff_Geom& diff_geom_, float *lod) const {
        Color kr(m_kr);
        if (hasTexture()){
            *lod = mTexture->computeMipmapLevel(diff_geom_.dTdx(), diff_geom_.dTdy());
            kr = getTextureColor(diff_geom_.u(), diff_geom_.v(),*lod);
        }
        return BSDF::init_fresnel (diff_geom_.normal(), kr, m_kt, m_ior);
	}
};
