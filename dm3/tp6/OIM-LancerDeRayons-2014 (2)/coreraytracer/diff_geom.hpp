#pragma once

#include "vector3d.hpp"
#include <memory>

/** @ingroup Raytracer 
 *  Geometrical information
 */
struct Diff_Geom {
private:
    /* surface differential  */
	Vector3D m_pos;
	Vector3D m_normal;
    Vector3D m_dNdx;
    Vector3D m_dNdy;

    /* Ray differentials */
    float m_t;
    Vector3D m_dPdx;
    Vector3D m_dPdy;
    Vector3D m_dDdx;
    Vector3D m_dDdy;

    /* Texture differential */
    float m_u;
    float m_v;
    Vector3D m_dTdx; //(dudx, dvdx, 0)
    Vector3D m_dTdy; //(dudy, dvdy, 0)

    /* in or out of the surface ? */
    bool m_in;

public:

    Diff_Geom (): m_pos (0.f), m_normal (0.f), m_dNdx(0.f), m_dNdy(0.f),
                  m_t(0.f), m_dPdx(0.f), m_dPdy(0.f), m_dDdx(0.f), m_dDdy(0.f),
                  m_u(0.f), m_v(0.f), m_dTdx(0.f), m_dTdy(0.f),
                  m_in(true) {
	} 

    Diff_Geom (const Vector3D& pos_, const Vector3D& normal_, const Vector3D& dndx_, const Vector3D& dndy_,
               float t, const Vector3D& dPdx_, const Vector3D& dPdy_, const Vector3D& dDdx_, const Vector3D& dDdy_,
               float u, float v, const Vector3D& dTdx_, const Vector3D& dTdy_,
               bool in) :
                          m_pos (pos_), m_normal (normal_), m_dNdx(dndx_), m_dNdy(dndy_),
                          m_t(t), m_dPdx(dPdx_), m_dPdy(dPdy_), m_dDdx(dDdx_), m_dDdy(dDdy_),
                          m_u(u), m_v(v), m_dTdx(dTdx_), m_dTdy(dTdy_),
                          m_in(in) {
	}

    Diff_Geom& operator=(const Diff_Geom& other) {
                    m_pos = other.m_pos;
                    m_normal = other.m_normal;
                    m_dNdx = other.m_dNdx;
                    m_dNdy = other.m_dNdy;

                    m_t = other.m_t;
                    m_dPdx = other.m_dPdx;
                    m_dPdy = other.m_dPdy;
                    m_dDdx = other.m_dDdx;
                    m_dDdy = other.m_dDdy;

                    m_u = other.m_u;
                    m_v = other.m_v;
                    m_dTdx = other.m_dTdx;
                    m_dTdy = other.m_dTdy;

                    m_in = other.m_in;
                    return (*this);
    }

	const Vector3D& pos () const {
		return m_pos;
	}

	const Vector3D& normal () const {
		return m_normal;
	}

    float t() const {
        return m_t;
    }

    Vector3D dPdx() const{
        return m_dPdx;
    }
    void set_dPdx(const Vector3D &dPdx){
        m_dPdx = dPdx;
    }

    Vector3D dPdy() const{
        return m_dPdy;
    }
    void set_dPdy(const Vector3D &dPdy){
        m_dPdy = dPdy;
    }

    Vector3D dDdx() const{
        return m_dDdx;
    }
    void set_dDdx(const Vector3D &dDdx){
        m_dDdx = dDdx;
    }

    Vector3D dDdy() const{
        return m_dDdy;
    }
    void set_dDdy(const Vector3D &dDdy){
        m_dDdy = dDdy;
    }

    float u() const {
        return m_u;
    }

    float v() const {
        return m_v;
    }

    Vector3D dTdx() const{
        return m_dTdx;
    }
    void set_dTdx(const Vector3D &dTdx){
        m_dTdx = dTdx;
    }

    Vector3D dTdy() const{
        return m_dTdy;
    }
    void set_dTdy(const Vector3D &dTdy){
        m_dTdy = dTdy;
    }

    bool in() const {
        return m_in;
    }

    void set_pos(const Vector3D & pos_) {
        m_pos = pos_;
    }

    void set_normal(const Vector3D & normal_) {
        m_normal = normal_;
    }

    void set_t(float t) {
        m_t = t;
    }

    void set_u(float u) {
        m_u = u;
    }
    void set_v(float v) {
        m_v = v;
    }

    void set_in(bool in) {
        m_in = in;
    }

    Vector3D dNdx() const {
        return m_dNdx;
    }

    Vector3D dNdy() const {
        return m_dNdy;
    }

    void set_dNdx(const Vector3D &dp) {
        m_dNdx = dp;
    }

    void set_dNdy(const Vector3D &dp) {
        m_dNdy = dp;
    }

};

//typedef std::   shared_ptr<Diff_Geom> PtrDiffData;



