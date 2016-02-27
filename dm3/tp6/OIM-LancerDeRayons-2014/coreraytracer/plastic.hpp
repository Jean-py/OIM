#pragma once

#include "material.hpp"

/** @ingroup Raytracer 
 *  Plastic material
 */
class Plastic : public Material {
private:

	Color m_kd;
	Color m_ks;
	float m_shininess;
	
public:

        Plastic (const Color& kd_, const Color& ks_, float shininess_) : Material(), m_kd (kd_), m_ks (ks_), m_shininess (shininess_) {
	}

    BSDF evaluate (const Diff_Geom& diff_geom_, float *lod) const {
        Color kd(m_kd);
        *lod = 1.;
        if (hasTexture()){
            *lod = mTexture->computeMipmapLevel(diff_geom_.dTdx(), diff_geom_.dTdy());
            kd = m_kd * getTextureColor(diff_geom_.u(), diff_geom_.v(),*lod);
            //kd = Color(diff_geom_.dNdx().norm());
        }
        return BSDF::init_blinn_phong (diff_geom_.normal(), kd, m_ks, m_shininess);
	}
};
