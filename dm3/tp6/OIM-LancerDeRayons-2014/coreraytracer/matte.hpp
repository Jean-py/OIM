#pragma once

#include "material.hpp"

/** @ingroup Raytracer 
 *  Matte material
 */
class Matte : public Material {
private:

	Color m_kd;
	
public:

    Matte (const Color& kd_) : Material(), m_kd (kd_) {
	}

    BSDF evaluate (const Diff_Geom& diff_geom_, float *lod) const {
        Color kd(m_kd);
        if (hasTexture()){
            *lod = mTexture->computeMipmapLevel(diff_geom_.dTdx(), diff_geom_.dTdy());
            kd = getTextureColor(diff_geom_.u(), diff_geom_.v(),*lod);
        }
        return BSDF::init_diffuse (diff_geom_.normal(), kd);
	}
};
