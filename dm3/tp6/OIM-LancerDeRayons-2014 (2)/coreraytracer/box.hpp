#pragma once

#include "geometry.hpp"
#include <cmath>

/** @ingroup Raytracer
 *  Box geometric object
 */

// TODO : compute ray differentials
class Box3D : public Geometry {
private:

    Vector3D m_center;
    Vector3D m_axis[3];
    Vector3D m_extent;

    bool clip(float denom, float numer, float& t0, float& t1, unsigned char &updated) const{
        // Return value is 'true' if line segment intersects the current test
        // plane.  Otherwise 'false' is returned in which case the line segment
        // is entirely clipped.
        updated = 0;
        if (denom > 0.f)  {
            if (numer > denom*t1) {
                return false;
            }
            if (numer > denom*t0) {
                t0 = numer/denom;
                updated = 1;
            }
            return true;
        } else if (denom < 0.f) {
            if (numer > denom*t0) {
                return false;
            }
            if (numer > denom*t1) {
                updated = 2;
                t1 = numer/denom;
            }
            return true;
        } else {
            return numer <= 0.f;
        }
    }

public:

    Box3D () : m_extent(0.5f, 0.5f, 0.5f) {
        m_axis[0] = Vector3D(1.f, 0.f, 0.f);
        m_axis[1] = Vector3D(0.f, 1.f, 0.f);
        m_axis[2] = Vector3D(0.f, 0.f, 1.f);
    }

    Box3D (const Vector3D& center_, const Vector3D &x_base_, const Vector3D &y_base_, const Vector3D &sizes_) : m_center (center_), m_extent(sizes_*0.5f) {
        m_axis[0] = x_base_.normalize();
        m_axis[1] = y_base_.normalize();
        m_axis[2] = m_axis[0].cross_product(m_axis[1]);
        m_axis[1] = m_axis[0].cross_product(m_axis[2]);

    }

    /**
       modifies the ray_.max_t() value, intersect only if the intersection
       is before ray_.max_t()
       Adapted from http://www.geometrictools.com/LibMathematics/Intersection/Wm5IntrLine3Box3.cpp

    */
    bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const {
        float t0 = -ray_.ray_epsilon();
        float t1 = ray_.max_t();

        // Convert linear component to box coordinates.
        Vector3D diff = ray_.ori() - m_center;
        Vector3D BOrigin(
            diff.dot(m_axis[0]),
            diff.dot(m_axis[1]),
            diff.dot(m_axis[2])
        );
        Vector3D BDirection(
            ray_.dir().dot(m_axis[0]),
            ray_.dir().dot(m_axis[1]),
            ray_.dir().dot(m_axis[2])
        );

        float saveT0 = t0, saveT1 = t1;
        bool notAllClipped;
        unsigned char updated;
        int nt0, nt1;
        notAllClipped = clip(+BDirection.x(), -BOrigin.x()-m_extent[0], t0, t1, updated);
        if (updated == 1)
            nt0 = 0;
        else if (updated == 2)
            nt1 = 0;
        notAllClipped = notAllClipped && clip(-BDirection.x(), +BOrigin.x()-m_extent[0], t0, t1, updated);
        if (updated == 1)
            nt0 = 1;
        else if (updated == 2)
            nt1 = 1;
        notAllClipped = notAllClipped && clip(+BDirection.y(), -BOrigin.y()-m_extent[1], t0, t1, updated);
        if (updated == 1)
            nt0 = 2;
        else if (updated == 2)
            nt1 = 2;
        notAllClipped = notAllClipped && clip(-BDirection.y(), +BOrigin.y()-m_extent[1], t0, t1, updated);
        if (updated == 1)
            nt0 = 3;
        else if (updated == 2)
            nt1 = 3;
        notAllClipped = notAllClipped && clip(+BDirection.z(), -BOrigin.z()-m_extent[2], t0, t1, updated);
        if (updated == 1)
            nt0 = 4;
        else if (updated == 2)
            nt1 = 4;
        notAllClipped = notAllClipped && clip(-BDirection.z(), +BOrigin.z()-m_extent[2], t0, t1, updated);
        if (updated == 1)
            nt0 = 5;
        else if (updated == 2)
            nt1 = 5;


        if (notAllClipped && (t0 != saveT0 || t1 != saveT1)) {

             if ( (t0 > ray_.ray_epsilon() ) && (t0 < ray_.max_t()) ){
                Vector3D normal = m_axis[nt0/2];
                if (nt0%2)
                    normal = -normal;
                p_diff_geom_->set_pos(ray_.at(t0));
                p_diff_geom_->set_normal(normal);
                p_diff_geom_->set_t(t0);
                p_diff_geom_->set_in(true);
                ray_.set_max_t(t0);
                return true;
            } else if ( (t1 > ray_.ray_epsilon()) && (t1 < ray_.max_t()) ){
                 Vector3D normal = m_axis[nt1/2];
                 if (nt1%2)
                     normal = -normal;
                 p_diff_geom_->set_pos(ray_.at(t1));
                 p_diff_geom_->set_normal(normal);
                 p_diff_geom_->set_t(t1);
                 p_diff_geom_->set_in(true);
                 ray_.set_max_t(t1);
                 return true;
             } else
                return false;
        } else {
            return false;
        }
    }

    bool clip(Ray& ray_, IntervalSet &bounds) const {
        float t0 = std::numeric_limits<float>::min();
        float t1 = std::numeric_limits<float>::max();
        Vector3D zero;

        // Convert linear component to box coordinates.
        Vector3D diff = ray_.ori() - m_center;
        Vector3D BOrigin(
            diff.dot(m_axis[0]),
            diff.dot(m_axis[1]),
            diff.dot(m_axis[2])
        );
        Vector3D BDirection(
            ray_.dir().dot(m_axis[0]),
            ray_.dir().dot(m_axis[1]),
            ray_.dir().dot(m_axis[2])
        );

        float saveT0 = t0, saveT1 = t1;
        bool notAllClipped;
        unsigned char updated;
        int nt0, nt1;
        notAllClipped = clip(+BDirection.x(), -BOrigin.x()-m_extent[0], t0, t1, updated);
        if (updated == 1)
            nt0 = 0;
        else if (updated == 2)
            nt1 = 0;
        notAllClipped = notAllClipped && clip(-BDirection.x(), +BOrigin.x()-m_extent[0], t0, t1, updated);
        if (updated == 1)
            nt0 = 1;
        else if (updated == 2)
            nt1 = 1;
        notAllClipped = notAllClipped && clip(+BDirection.y(), -BOrigin.y()-m_extent[1], t0, t1, updated);
        if (updated == 1)
            nt0 = 2;
        else if (updated == 2)
            nt1 = 2;
        notAllClipped = notAllClipped && clip(-BDirection.y(), +BOrigin.y()-m_extent[1], t0, t1, updated);
        if (updated == 1)
            nt0 = 3;
        else if (updated == 2)
            nt1 = 3;
        notAllClipped = notAllClipped && clip(+BDirection.z(), -BOrigin.z()-m_extent[2], t0, t1, updated);
        if (updated == 1)
            nt0 = 4;
        else if (updated == 2)
            nt1 = 4;
        notAllClipped = notAllClipped && clip(-BDirection.z(), +BOrigin.z()-m_extent[2], t0, t1, updated);
        if (updated == 1)
            nt0 = 5;
        else if (updated == 2)
            nt1 = 5;

        if (notAllClipped && (t0 != saveT0 || t1 != saveT1)) {
            if (t1 > t0) {

                Vector3D normal = m_axis[nt0/2];
                if (nt0%2)
                    normal = -normal;

//                Diff_Geom *in = new Diff_Geom(ray_.at(t0), normal, zero, zero, t0, true, 0, 0);
                Diff_Geom *in = new Diff_Geom(ray_.at(t0), normal, zero, zero, t0, zero, zero, zero, zero, 0, 0, zero, zero, true);

                normal = m_axis[nt1/2];
                if (nt1%2)
                    normal = -normal;
//                Diff_Geom *out = new Diff_Geom(ray_.at(t1), normal, zero, zero, t1, false, 0, 0);
                Diff_Geom *out = new Diff_Geom(ray_.at(t1), normal, zero, zero, t1, zero, zero, zero, zero, 0, 0, zero, zero, false);
                bounds.add(in, out);
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

};
