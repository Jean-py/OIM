#pragma once

#include "color.hpp"
#include "vector3d.hpp"


/** @ingroup Raytracer 
 *  Light source definition
 */
class Light {
private:

	Vector3D m_pos;

	/** emitted radiance */
	Color m_le;
	
public:

	Light () {
	}

	Light (const Vector3D& pos_, const Color& le_) : m_pos (pos_), m_le (le_) {
	}

	const Vector3D& pos () const {
		return m_pos;
	}

	const Color& le() const {
		return m_le;
	}
};
