#pragma once

#include <cmath>
#include <iostream>

/** @ingroup Raytracer 
 *  3D vector manipulation
 */
class Vector3D {
private:

	/** 0 = x, 1 = y, 2 = z */
	float m_comps[3];

public:

	Vector3D () {
		m_comps[0] = 0.f;
		m_comps[1] = 0.f;
		m_comps[2] = 0.f;
	}

	Vector3D (float v_) {
		m_comps[0] = v_;
		m_comps[1] = v_;
		m_comps[2] = v_;
	}

	Vector3D (float x_, float y_, float z_) {
		m_comps[0] = x_;
		m_comps[1] = y_;
		m_comps[2] = z_;
	}

	float x() const {
		return m_comps[0];
	}

	float y() const {
		return m_comps[1];
	}
	
	float z() const {
		return m_comps[2];
	}

        float operator[] (int i_) const {
		return m_comps[i_];
	}

        float &operator[] (int i_) {
                return m_comps[i_];
        }

	Vector3D operator- () const {
		return Vector3D (-m_comps[0], -m_comps[1], -m_comps[2]);
	}

	Vector3D operator+ (const Vector3D& other_) const {
		return Vector3D (m_comps[0] + other_.m_comps[0],
						 m_comps[1] + other_.m_comps[1],
						 m_comps[2] + other_.m_comps[2]);
	}

	Vector3D operator- (const Vector3D& other_) const {
		return Vector3D (m_comps[0] - other_.m_comps[0],
						 m_comps[1] - other_.m_comps[1],
						 m_comps[2] - other_.m_comps[2]);
	}

	Vector3D operator* (const Vector3D& other_) const {
		return Vector3D (m_comps[0] * other_.m_comps[0],
						 m_comps[1] * other_.m_comps[1],
						 m_comps[2] * other_.m_comps[2]);		
	}

	Vector3D operator* (float v_) const {
		return Vector3D (m_comps[0] * v_,
						 m_comps[1] * v_,
						 m_comps[2] * v_);
	}


	float norm () const {
        return std::sqrt (x() * x() + y() * y() + z() * z());
	}

	float dot (const Vector3D& other_) const {
		return x() * other_.x() + y() * other_.y() + z() * other_.z();
	}

	Vector3D normalize () const {
		return (*this) * (1.f / norm());
	}

	Vector3D cross_product (const Vector3D& other_) const {
		return Vector3D (y() * other_.z() - z() * other_.y(),
						 z() * other_.x() - x() * other_.z(),
						 x() * other_.y() - y() * other_.x());
	}

    Vector3D get_perpendicular() const {
        Vector3D candidates[2];
        candidates[0][0] = m_comps[2];
        candidates[0][1] = m_comps[2];
        candidates[0][2] = -m_comps[0]-m_comps[1];

        candidates[1][0] = -m_comps[1]-m_comps[2];
        candidates[1][1] = m_comps[0];
        candidates[1][2] = m_comps[0];

        int selectIndex = ((m_comps[2] != 0) && (-m_comps[0] != m_comps[1]));
        return candidates[selectIndex];
    }

    static void make_basis(Vector3D &u, Vector3D &v, const Vector3D & w){
        float invLength;
        if (std::fabs(w[0]) >= std::fabs(w[1])) {
            // W.x or W.z is the largest magnitude component, swap them
            invLength = 1.f/std::sqrt(w[0]*w[0] + w[2]*w[2]);
            u[0] = -w[2]*invLength;
            u[1] = 0.f;
            u[2] = +w[0]*invLength;
            v[0] = w[1]*u[2];
            v[1] = w[2]*u[0] - w[0]*u[2];
            v[2] = -w[1]*u[0];
        } else {
            // W.y or W.z is the largest magnitude component, swap them
            invLength = 1.f/std::sqrt(w[1]*w[1] + w[2]*w[2]);
            u[0] = 0.f;
            u[1] = +w[2]*invLength;
            u[2] = -w[1]*invLength;
            v[0] = w[1]*u[2] - w[2]*u[1];
            v[1] = -w[0]*u[2];
            v[2] = w[0]*u[1];
        }
    }

    friend std::ostream & operator<<(std::ostream &os, Vector3D v){
        os << v.m_comps[0] << " " <<  v.m_comps[1] << " " << v.m_comps[2];
        return os;
    }
};


