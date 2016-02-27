#pragma once

#include "color.hpp"
#include "vector3d.hpp"

/** @ingroup Raytracer 
 *  Reflectance representation and evaluation
 */class BSDF {
private:

	/** points toward the outside of the object */
	Vector3D m_normal;

	typedef enum {
		DIFFUSE,
		BLINN_PHONG,
        GLOSSY,
		PURE_REFL,
		FRESNEL		
	} Model;

	Model m_model;

	/** kd for diffuse, phong and pure_refl, divided by pi for normalization */
	Color m_kd;

	/** ks and shininess for phong */
	Color m_ks;
	float m_shininess;

	/** reflection, refraction and ior for fresnel */
	Color m_kr, m_kt;
	float m_ior;

	/**
	   meta-constructor
	*/
	BSDF (const Vector3D& normal_, const Model& model_, const Color& kd_,
		  const Color& ks_, float shininess_, const Color& kr_, const Color& kt_,
		  float ior_)
		: m_normal (normal_), m_model (model_), m_kd (kd_ * (1.f / M_PI)), m_ks (ks_),
		  m_shininess (shininess_), m_kr (kr_), m_kt (kt_), m_ior (ior_) {
	}
		  

public:

	BSDF () : m_model (DIFFUSE), m_shininess (0.f), m_ior (0.f) {
	}

	//==========
	//factories
	
	static BSDF init_diffuse (const Vector3D& normal_, const Color& kd_) {
		return BSDF (normal_, DIFFUSE,
					 kd_, Color (0.f), 0.f,
					 Color (0.f), Color (0.f), 0.f);
	}

	static BSDF init_blinn_phong (const Vector3D& normal_, const Color& kd_,
								  const Color& ks_, float shininess_) {
		return BSDF (normal_, BLINN_PHONG,
					 kd_, ks_, shininess_,
					 Color (0.f), Color (0.f), 0.f);
	}

	static BSDF init_pure_refl (const Vector3D& normal_, const Color& kd_, const Color& kr_) {
		return BSDF (normal_, PURE_REFL,
					 kd_, Color (0.f), 0.f,
					 kr_, Color (0.f), 1.3f); /** hack for the IOR, should use different Fresnel equations */
	}

    static BSDF init_fresnel (const Vector3D& normal_, const Color& kr_, const Color& kt_, float ior_) {
        return BSDF (normal_, FRESNEL,
                     Color (0.f), Color (0.f), 0.f,
                     kr_, kt_, ior_);
    }

    static BSDF init_glossy (const Vector3D& normal_, const Color& kd_,
                                  const Color& ks_, const Color& kr_, float shininess_, float ior_) {
        return BSDF (normal_, GLOSSY,
                     kd_, ks_, shininess_,
                     kr_, Color (0.f), ior_);
    }

	//==========
	//evaluation

	/**
	   directions leave from the point
	*/
	Color evaluate (const Vector3D& to_eye_, const Vector3D& to_light_) const {
		switch (m_model) {
		case DIFFUSE:
			if (m_normal.dot (to_eye_) * m_normal.dot (to_light_) < 0.f) {
				//not same hemisphere
				return Color (0.f);
			}
			return m_kd;
			break;
		case BLINN_PHONG:
		{
			if (m_normal.dot (to_eye_) * m_normal.dot (to_light_) < 0.f) {
				//not same hemisphere
				return Color (0.f);
			}
			//www.cs.ucl.ac.uk/staff/j.kautz/GameCourse/04_PointLights.pdf
			Vector3D half = (to_eye_ + to_light_).normalize();
            return m_kd + m_ks * (std::pow ((half.dot (m_normal)), m_shininess) * (m_shininess * 4) / (8.f * M_PI));
			break;
		}
        case GLOSSY:
        {
                if (m_normal.dot (to_eye_) * m_normal.dot (to_light_) < 0.f) {
                    //not same hemisphere
                    return Color (0.f);
                }
                float r0 = (1-m_ior)/(1+m_ior);
                r0 = r0 * r0;
                Vector3D half = (to_eye_ + to_light_).normalize();
                float costetha = half.dot(to_eye_);
                float r = r0 + (1-r0)*std::pow((1-costetha), 5.);
                //www.cs.ucl.ac.uk/staff/j.kautz/GameCourse/04_PointLights.pdf
                return (m_kd*(1./(M_PI)) + m_ks * r * (std::pow ((half.dot (m_normal)), m_shininess) * (m_shininess * 4) / (8.f * M_PI)));
                //return m_kd*((r)/M_PI);

            break;
        }
        case PURE_REFL:
			return m_kd; //no chance it is a reflection direction
			break;
        case FRESNEL:
            return Color (0.f); //no chance it is a reflection direction
            break;
		default:
			return Color (0.f);			
		}
	}

	/*************/
	bool has_reflection () const {
        return (m_model == FRESNEL) || (m_model == PURE_REFL) || (m_model == GLOSSY);
	}

	/*************/
	bool has_refraction () const {
		return m_model == FRESNEL;
	}

	/*************/
	Color reflect_dir (const Vector3D& to_eye_, Vector3D * p_to_light_) const {

		float cos_theta = m_normal.dot (to_eye_);
				
		*p_to_light_ = -to_eye_ + m_normal * 2.f * cos_theta; //not abs_cos_theta, the normal may be in the wrong sense
		
		bool entering = (cos_theta > 0.f);
		float ior_i, ior_t;

		if (entering) {
			ior_i = 1.f; //assume non-nested refractive objects
			ior_t = m_ior;
		} else {
			ior_i = m_ior;
			ior_t = 1.f;
		}

		float ior_ratio = ior_i / ior_t;

		float cos_squared = 1.f - ior_ratio * ior_ratio * (1.f - cos_theta * cos_theta);

		if (cos_squared < 0.f) {
			//no transmitted media, it is a total internal reflection
			//(light is totally reflected instead of being transmitted).
			return m_kr;
		}
		
		//compute fresnel reflectance.		
		float ref_fresnel_0 = (ior_i - ior_t) / (ior_i + ior_t);
		ref_fresnel_0 *= ref_fresnel_0;

		float abs_cos_theta = fabsf (cos_theta);
        float ref_fresnel = ref_fresnel_0 + (1.f - ref_fresnel_0) * std::pow (1.f - abs_cos_theta, 5.f);

		return m_kr * ref_fresnel;
	}

	/*************/
	Color refract_dir (const Vector3D& to_eye_, Vector3D * p_to_light_) const {
		float cos_theta = m_normal.dot (to_eye_);
		
		bool entering = (cos_theta > 0.f);
		float ior_i, ior_t;

		if (entering) {
			ior_i = 1.f; //assume non-nested refractive objects
			ior_t = m_ior;
		} else {
			ior_i = m_ior;
			ior_t = 1.f;
		}

		float ior_ratio = ior_i / ior_t;
		
		float cos_squared = 1.f - ior_ratio * ior_ratio * (1.f - cos_theta * cos_theta);

		if (cos_squared < 0.f) {
			//no transmitted media, it is a total internal reflection
			//(light is totally reflected instead of being transmitted).
			return Color (0.f);
		}

        Vector3D normal_part = m_normal * (ior_ratio * cos_theta - (entering ? 1.f : -1.f) * std::sqrt (cos_squared));
		Vector3D eye_part = to_eye_ * ior_ratio;
		*p_to_light_ = normal_part - eye_part;
		
		float ref_fresnel_0 = (ior_i - ior_t) / (ior_i + ior_t);
		ref_fresnel_0 *= ref_fresnel_0;

        float ref_fresnel = ref_fresnel_0 + (1.f - ref_fresnel_0) * std::pow (1.f - fabsf (cos_theta), 5.f);

		return m_kt * (1.f - ref_fresnel);
	}
		
    float getIor(bool entering) const{
        if (entering)
            return 1.f/m_ior;
        else
            return m_ior;
    }

};
