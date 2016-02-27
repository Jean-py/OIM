#pragma once

#include "vector3d.hpp"
#include "ray.hpp"

/** @ingroup Raytracer 
 *  Viewpoint management
 */
class Camera {
private:

	/** position of the observator, starting point of the rays */
	Vector3D m_pos; 
	
	/** center of image plane */
	Vector3D m_center; 

    Vector3D m_x, m_y, m_z;

	int m_x_res, m_y_res;

public:

	Camera () : m_x_res (0), m_y_res (0) {
	}

	/**
	   fov in degrees
	*/
	Camera (const Vector3D& pos_, const Vector3D& aim_, const Vector3D& up_,
			float fov_, int x_res_, int y_res_) : m_pos (pos_), m_x_res (x_res_), m_y_res (y_res_) {
        m_z = (aim_ - pos_).normalize();
        m_x = (up_.cross_product (m_z)).normalize();
        m_y = (m_z.cross_product (m_x)) * (float(y_res_) / float (x_res_)); //aspect ratio

        m_center = m_pos + m_z * (1.f / tanf ((fov_ * M_PI / 180.f) * 0.5f));
	}

	Ray ray_for_pixel (int x_, int y_) const {
		float delta_x = float((x_ + 0.5f) - m_x_res * 0.5f) / (m_x_res * 0.5f);
		float delta_y = float((y_ + 0.5f) - m_y_res * 0.5f) / (m_y_res * 0.5f);

        // initialize ray diffferentials
        Vector3D d = m_z + (m_x * delta_x) + (m_y * delta_y);
        Vector3D dPdx, dPdy, dDdx, dDdy;
        dDdx = (( (m_x * d.dot(d)) - (d * d.dot(m_x)) )*(1.f/std::pow(d.dot(d), 1.5))) * (1.f/((float)m_x_res*0.5f));
        dDdy = (( (m_y * d.dot(d)) - (d * d.dot(m_y)) )*(1.f/std::pow(d.dot(d), 1.5))) * (1.f/((float)m_y_res*0.5f));
		
        Ray theRay(m_pos, d.normalize());
        theRay.setdDdx(dDdx);
        theRay.setdDdy(dDdy);
        theRay.setdPdx(dPdx);
        theRay.setdPdy(dPdy);

        return theRay;
	}

	int x_res () const {
		return m_x_res;
	}

	int y_res () const {
		return m_y_res;
	}
};

	
