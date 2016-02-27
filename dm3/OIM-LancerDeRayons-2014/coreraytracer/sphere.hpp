#pragma once

#include "geometry.hpp"

/** @ingroup Raytracer 
 *  Sphere geometric object
 */
class Sphere : public Geometry {
private:

	Vector3D m_center;
	float m_radius;
	
    void computeTexInfo(const Vector3D &pt_inter, const Vector3D &normal, const Vector3D &dPpdx, const Vector3D &dPpdy, float &u, float &v, Vector3D &dTdx, Vector3D &dTdy) const{
        float theta = acosf(normal[2]);
        float phi = atan2f(normal[1], normal[0]);
        v = theta/M_PI;
        u = (phi+M_PI)/(2.f*M_PI);

        Vector3D dNdx = dPpdx*(1.f/m_radius);
        Vector3D dNdy = dPpdy*(1.f/m_radius);
        if (normal[0] != 0.f ){
            dTdx[0] = (1.f/(1.f + (normal[1]/normal[0])*(normal[1]/normal[0])) * (dNdx[1]*normal[0] + normal[1]*dNdx[0])/(normal[0]*normal[0]))/(2.f*M_PI);
            dTdx[1] = (dNdx[2]*(-1.f/std::sqrt(1.f-(normal[2]*normal[2]))))/M_PI;
        } else {
            // What else ?
            dTdx = Vector3D(M_PI);
        }

        if (normal[0] != 0.f ){
            dTdy[0] = (1.f/(1.f + (normal[1]/normal[0])*(normal[1]/normal[0])) * (dNdy[1]*normal[0] + normal[1]*dNdy[0])/(normal[0]*normal[0]))/(2.f*M_PI);
            dTdy[1] = (dNdy[2]*(-1.f/std::sqrt(1.f-(normal[2]*normal[2]))))/M_PI;
        } else {
            // What else ?
            dTdy = Vector3D(M_PI);
        }
    }

public:

	Sphere () : m_radius (0.f) {
	}

	Sphere (const Vector3D& center_, float radius_) : m_center (center_), m_radius (radius_) {
	}

	/**
	   modifies the ray_.max_t() value, intersect only if the intersection
	   is before ray_.max_t()
	*/
	bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const {
		Vector3D ori_local = ray_.ori() - m_center;

		float dir_dot_ori = ray_.dir().dot (ori_local);
		float delta = dir_dot_ori * dir_dot_ori - (ori_local.dot (ori_local) - m_radius * m_radius);

		if (delta < 0.f) {
			return false;
		}

        float sqrt_delta = std::sqrt (delta);

		float t;

		float t2 = -dir_dot_ori + sqrt_delta;

		//to avoid auto intersection
		if (t2 < Ray::ray_epsilon()) {
			//t1 < t2 => no solution after the origin of the ray
			return false;
		}

		float t1 = -dir_dot_ori - sqrt_delta;
        bool in = true;
        if (t1 > Ray::ray_epsilon()) {
			//t1 < t2
			t = t1;
		} else {
			t = t2;
            in = false;
		}

		if (t > ray_.max_t()) {
			return false;
		}

		ray_.set_max_t (t);

        Vector3D pt_inter = ray_.at(t);
        Vector3D normal = (pt_inter - m_center).normalize();

        // update ray differentials
        Vector3D dPpdx, dPpdy;
        ray_.propagateDifferentials(t, normal, dPpdx,dPpdy);
        // compute Ndx and dNdy
        Vector3D dNdx = dPpdx*(1.f/m_radius);
        Vector3D dNdy = dPpdy*(1.f/m_radius);
        // texture coordinate
        float u, v;
        Vector3D dTdx;
        Vector3D dTdy;
        computeTexInfo(pt_inter, normal, dPpdx, dPpdy, u, v, dTdx, dTdy);

        *p_diff_geom_ = Diff_Geom(pt_inter, normal, dNdx, dNdy, t, dPpdx, dPpdy, ray_.dDdx(), ray_.dDdy(), u, v, dTdx, dTdy, in);

		return true;
	}
    /**
     * @brief clip a ray by a geometry.
     * @param ray_
     * @param p_diff_geom_in_ The point by wich the ray enters the geometry
     * @param p_diff_geom_out_ The point by wich the ray leaves the geometry
     * @return true if the ray intersects the geometry. In and out parameters are filled by this method.
     * The default returned value is false such that non volumic geometries do not need to implement this.
     *
     */
    bool clip(Ray& ray_, IntervalSet &bounds) const {
        //std::cerr << "Clip Sphere" << std::endl;
        Vector3D ori_local = ray_.ori() - m_center;

        float dir_dot_ori = ray_.dir().dot (ori_local);
        float delta = dir_dot_ori * dir_dot_ori - (ori_local.dot (ori_local) - m_radius * m_radius);

        // do not consider double root
        if (delta <= 0.f) {
            return false;
        }

        float sqrt_delta = std::sqrt (delta);
        // t is the entry point
        float t = -dir_dot_ori - sqrt_delta;
        Vector3D pt_inter = ray_.at(t);
        Vector3D normal = (pt_inter - m_center).normalize();

        // update ray differentials
        Vector3D dPpdx, dPpdy;
        ray_.propagateDifferentials(t, normal, dPpdx,dPpdy);
        // compute Ndx and dNdy
        Vector3D dNdx = dPpdx*(1.f/m_radius);
        Vector3D dNdy = dPpdy*(1.f/m_radius);
        // texture coordinate
        float u, v;
        Vector3D dTdx;
        Vector3D dTdy;
        computeTexInfo(pt_inter, normal, dPpdx, dPpdy, u, v, dTdx, dTdy);

        Diff_Geom *in = new Diff_Geom(pt_inter, normal, dNdx, dNdy, t, dPpdx, dPpdy, ray_.dDdx(), ray_.dDdy(), u, v, dTdx, dTdy, true);



        t = -dir_dot_ori + sqrt_delta;
        // t is the leaving point
        pt_inter = ray_.at(t);
        normal = (pt_inter - m_center).normalize();

        // update ray differentials
        ray_.propagateDifferentials(t, normal, dPpdx,dPpdy);
        // compute Ndx and dNdy
        dNdx = dPpdx*(1.f/m_radius);
        dNdy = dPpdy*(1.f/m_radius);
        // texture coordinate
        computeTexInfo(pt_inter, normal, dPpdx, dPpdy, u, v, dTdx, dTdy);

        Diff_Geom *out = new Diff_Geom(pt_inter, normal, dNdx, dNdy, t, dPpdx, dPpdy, ray_.dDdx(), ray_.dDdy(), u, v, dTdx, dTdy, false);

        bounds.add(in, out);

        return true;
    }

};
