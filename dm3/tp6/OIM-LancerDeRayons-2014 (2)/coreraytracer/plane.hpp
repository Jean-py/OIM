#pragma once

#include "geometry.hpp"
#include <cmath>

/** @ingroup Raytracer 
 *  Plane geometric object
 */
class Plane : public Geometry {
private:

    Vector3D m_pos;
    Vector3D m_normal;
    Vector3D m_tangent;
    Vector3D m_binormal;
    float m_offset;
	
public:

    Plane (const Vector3D& pos_, const Vector3D& normal_) {
        m_pos = pos_;
        m_normal = normal_.normalize();
        m_tangent = m_normal.get_perpendicular();
        m_tangent = m_tangent.normalize();
        m_binormal = m_normal.cross_product(m_tangent);

        m_offset = -m_normal.dot (m_pos);
	}

    void settangent(const Vector3D &tg){
        m_tangent = tg;
        m_binormal = m_normal.cross_product(m_tangent);
    }

	/**
	   modifies the ray_.max_t() value, intersect only if the intersection
	   is before ray_.max_t()
	*/
	bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const {
        float d = ray_.dir().dot (m_normal);

        if (std::fabs(d) < ray_.ray_epsilon())
            return false;

        float t = (-m_offset - ray_.ori().dot (m_normal)) / d;

		if (t < Ray::ray_epsilon() || t > ray_.max_t()) {
			return false;
		}

		ray_.set_max_t (t);
        Vector3D pt_inter = ray_.at (t);

        // update ray differentials
        Vector3D dPpdx, dPpdy;
        ray_.propagateDifferentials(t, m_normal, dPpdx,dPpdy);

        // dNdx and dNdy are zero
        Vector3D zero;
        // texture coordinate
        Vector3D vec_inter = pt_inter - m_pos;
        float u = m_tangent.dot(vec_inter);
        float v = m_binormal.dot(vec_inter);
        // compute texture differential
        Vector3D dTdx;
        Vector3D dTdy;
        dTdx[0] = m_tangent.dot(dPpdx);
        dTdx[1] = m_binormal.dot(dPpdx);
        dTdy[0] = m_tangent.dot(dPpdy);
        dTdy[1] = m_binormal.dot(dPpdy);

         *p_diff_geom_ = Diff_Geom(pt_inter, m_normal, zero, zero, t, dPpdx, dPpdy, ray_.dDdx(), ray_.dDdy(), u, v, dTdx, dTdy, true);

		return true;
	}

    bool clip(Ray& ray_, IntervalSet &bounds) const {

        Diff_Geom *in = new Diff_Geom();
        intersect(ray_, in);
        Diff_Geom *out = new Diff_Geom(in->pos(), -in->normal(), in->dNdx(), in->dNdy(), in->t()+ray_.ray_epsilon(), in->dPdx(), in->dPdy(), in->dDdx(), in->dDdy(), in->u(), in->v(), in->dTdx(), in->dTdy(), false);
        bounds.add(in, out);
        return true;
    }
};
