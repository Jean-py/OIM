#pragma once

#include "material.hpp"

/** @ingroup Raytracer 
 *  Plastic material
 */
class Glossy : public Material {
private:

	Color m_kd;
    Color m_ks;
    Color m_kr;
    float m_ior;
    float m_shininess;

public:

        Glossy (const Color& kd_, const Color& ks_, const Color& kr_, float ior_, float shininess_) : Material(), m_kd (kd_), m_ks (ks_), m_kr (kr_), m_ior(ior_), m_shininess (shininess_) {
	}

    BSDF evaluate (const Diff_Geom& diff_geom_, float *lod) const {
        Color kd(m_kd);
        *lod = 1.;
        if (hasTexture()){
            *lod = mTexture->computeMipmapLevel(diff_geom_.dTdx(), diff_geom_.dTdy());
            kd = m_kd * getTextureColor(diff_geom_.u(), diff_geom_.v(),*lod);
        }
        return BSDF::init_glossy(diff_geom_.normal(), kd, m_ks, m_kr, m_shininess, m_ior);
	}
};
