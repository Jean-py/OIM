#pragma once

#include "geometry.hpp"
#include "material.hpp"
#include "isect.hpp"

#include <cstdlib>

/** @ingroup Raytracer 
 *  Primitive object : base component of a scene
 */
class Primitive {
private:

	Geometry * m_p_geom;
	Material * m_p_material;
	
public:

	Primitive () : m_p_geom (NULL), m_p_material (NULL) {
	}

	Primitive (Geometry * p_geom_,
			   Material * p_mat_) : m_p_geom (p_geom_),
									m_p_material (p_mat_) {
		//the primitive does not own the objects, they have to be released
		//outside (a material can be shared by several geometries for instance)
	}

	bool intersect (Ray& ray_, Isect * p_isect_) const {
		Diff_Geom dg;
		if (!m_p_geom->intersect (ray_, &dg)) {
			return false;
		}

        //BSDF bsdf = m_p_material->evaluate (dg, 1.f);

        *p_isect_ = Isect (dg, m_p_material);

		return true;
	}
};
