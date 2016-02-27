#pragma once

/** @ingroup Raytracer 
 *  Color management
 */
class Color {
private:

	/** 0 = r, 1 = g, 2 = b */
	float m_comps[3]; 


public:

	Color () {
		m_comps[0] = 0.f;
		m_comps[1] = 0.f;
		m_comps[2] = 0.f;
	}

    Color (const Color& other) {
        m_comps[0] = other.m_comps[0];
        m_comps[1] = other.m_comps[1];
        m_comps[2] = other.m_comps[2];
    }

	Color (float v_) {
		m_comps[0] = v_;
		m_comps[1] = v_;
		m_comps[2] = v_;
	}

	Color (float r_, float g_, float b_) {
		m_comps[0] = r_;
		m_comps[1] = g_;
		m_comps[2] = b_;
	}

	float operator[] (int i_) const {
		return m_comps[i_];
	}

	Color operator+ (const Color& other_) const {
		return Color (m_comps[0] + other_.m_comps[0],
					  m_comps[1] + other_.m_comps[1],
					  m_comps[2] + other_.m_comps[2]);
	}

	Color operator* (const Color& other_) const {
		return Color (m_comps[0] * other_.m_comps[0],
					  m_comps[1] * other_.m_comps[1],
					  m_comps[2] * other_.m_comps[2]);		
	}

	Color operator* (float v_) const {
		return Color (m_comps[0] * v_,
					  m_comps[1] * v_,
					  m_comps[2] * v_);
	}

    friend Color operator*(float v_, const Color&c){
        return Color(c*v_);
    }

	bool is_zero () const {
		return ((m_comps[0] == 0.f) & (m_comps[1] == 0.f) & (m_comps[2] == 0.f));
	}

	float avg () const {
		return (m_comps[0] + m_comps[1] + m_comps[2]) * 0.3333f;
	}
};
