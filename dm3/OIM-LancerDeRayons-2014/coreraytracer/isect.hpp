#pragma once

#include "diff_geom.hpp"
//#include "bsdf.hpp"
#include "material.hpp"


/** @ingroup Raytracer 
 *  Intersection point definition 
 */
struct Isect {
private:
	Diff_Geom m_dg;
    Material *m_mat;
    BSDF m_bsdf;


public:

	Isect () {
	}

    Isect (const Diff_Geom& dg_, Material *mat_) : m_dg (dg_), m_mat (mat_) {
	}

	const Diff_Geom& dg() const {
		return m_dg;
	}

    const Material& mat() const {
        return *m_mat;
	}

    void setBsdf( const BSDF &bsdf){
        m_bsdf = bsdf;
    }

    const BSDF &bsdf() const {
        return m_bsdf;
    }
};

	
