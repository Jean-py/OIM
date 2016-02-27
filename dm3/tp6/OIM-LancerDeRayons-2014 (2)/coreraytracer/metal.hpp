#pragma once

#include "material.hpp"

/** @ingroup Raytracer 
 *  Metallic material
 */
class Metal : public Material {
private:

	Color m_kd, m_kr;
	
public:

    Metal (const Color& kd_, const Color& kr_) : Material(),  m_kd (kd_), m_kr (kr_) {
	}

    BSDF evaluate (const Diff_Geom& diff_geom_, float *lod) const {
        Color kd(m_kd);
        if (hasTexture()){
            *lod = mTexture->computeMipmapLevel(diff_geom_.dTdx(), diff_geom_.dTdy());
            kd = getTextureColor(diff_geom_.u(), diff_geom_.v(),*lod);
        }
        return BSDF::init_pure_refl (diff_geom_.normal(), kd, m_kr);
	}
};
