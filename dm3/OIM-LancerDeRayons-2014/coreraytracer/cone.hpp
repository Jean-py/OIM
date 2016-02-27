#pragma once

#include "geometry.hpp"
#include "plane.hpp"
#include <cmath>
#include <cassert>

#define PSCALE 8.
/** @ingroup Raytracer
 *  Cone geometric object
 */

class Cone : public Geometry {
private:
    /// The position of the vertex
    Vector3D m_vertex;
    /// The axis of the cone
    Vector3D m_axis;
    /// The radius at the base
    float m_radius;
    /// The height of the cone
    float m_height;

    /// The cos(angle) of the cone
    float m_costheta;

    Vector3D U, V, W;


    void computeConicalTexInfo(const Vector3D &pt_inter, const Vector3D &normal, const Vector3D &dPpdx, const Vector3D &dPpdy, float &u, float &v, Vector3D &dTdx, Vector3D &dTdy) const{

        Vector3D ptOnCone = pt_inter - m_vertex;
        // from cone --> to integrate
        float da = ptOnCone.norm();
        float dt = std::sqrt(m_radius*m_radius+m_height*m_height);

        float radiusAtPt = da*m_radius/dt;
        Vector3D ptOnConeBis(U.dot(ptOnCone), V.dot(ptOnCone), W.dot(ptOnCone));
        ptOnCone[0] = ptOnConeBis[0];//ptOnCone[0]/radiusAtPt;
        ptOnCone[1] = ptOnConeBis[1];//ptOnCone[1]/radiusAtPt;
        // texture coordinates
        u = (std::atan2(ptOnCone[1],ptOnCone[0])+M_PI)/(2.f*M_PI);
        v = ((dt-da)/dt);

        // Texture differential
        dTdx[0] = (1.f/(2.f*M_PI)) * (1.f/(1.f + std::pow(ptOnCone[1]/ptOnCone[0], 2.f))) *
               (
                    ((dPpdx[1]*radiusAtPt - ptOnCone[1]*(m_radius/(dt*da)))/std::pow(radiusAtPt, 2.f)) * ptOnCone[0]
                    - ptOnCone[1]*((dPpdx[0]*radiusAtPt - ptOnCone[0]*(m_radius/(dt*da)))/std::pow(radiusAtPt, 2.f))
               )/std::pow(ptOnCone[0], 2.f);

        dTdy[0] = (1.f/(2.f*M_PI)) * (1.f/(1.f + std::pow(ptOnCone[1]/ptOnCone[0], 2.f))) *
               (
                    ((dPpdy[1]*radiusAtPt - ptOnCone[1]*(m_radius/(dt*da)))/std::pow(radiusAtPt, 2.f)) * ptOnCone[0]
                    - ptOnCone[1]*((dPpdy[0]*radiusAtPt - ptOnCone[0]*(m_radius/(dt*da)))/std::pow(radiusAtPt, 2.f))
               )/std::pow(ptOnCone[0], 2.f);
        float coef = (1.f / (dt*da));
        dTdx[1] = coef * dPpdx.dot(ptOnCone);
        dTdy[1] = coef * dPpdy.dot(ptOnCone);
    }

    void fillConicalDiffGeom(Diff_Geom &dg, const Ray &r, float t, bool in) const{
        Vector3D pt_inter = r.at(t);
        Vector3D dNdx, dNdy;
        Vector3D normal=computenormal(pt_inter);
        Vector3D dPpdx, dPpdy;
        r.propagateDifferentials(t, normal, dPpdx, dPpdy);
        computenormalDifferential(pt_inter, dPpdx, dPpdy, dNdx, dNdy);
        Vector3D dTdx, dTdy;
        float u, v;
        computeConicalTexInfo(pt_inter, normal, dPpdx, dPpdy, u, v, dTdx, dTdy);
        dg = Diff_Geom(pt_inter, normal, dNdx, dNdy, t, dPpdx, dPpdy, r.dDdx(), r.dDdy(), u, v, dTdx, dTdy, in);
    }

    Vector3D computenormal(const Vector3D & p_) const{
        // compute the cone normal at point p_
        Vector3D e = p_ - m_vertex;
        Vector3D h = e.cross_product(m_axis);
        Vector3D n = e.cross_product(h);
        return n.normalize();
    }

    Vector3D computenormalDifferential(const Vector3D & p_, Vector3D &dPpdx, Vector3D &dPpdy, Vector3D &dNdx, Vector3D &dNdy) const{
        // compute the cone normal at point
        Vector3D e = p_ - m_vertex;
        Vector3D h = e.cross_product(m_axis);
        Vector3D n = e.cross_product(h);
        float ndotn = n.dot(n);
        // compute Ndx and dNdy


        Vector3D dnpdx = dPpdx.cross_product(h)+e.cross_product(dPpdx.cross_product(m_axis));
        Vector3D dnpdy = dPpdy.cross_product(h)+e.cross_product(dPpdy.cross_product(m_axis));
        float fnpx = -std::pow(ndotn, -1.5f) * dnpdx.dot(n);
        float fnpy = -std::pow(ndotn, -1.5f) * dnpdy.dot(n);
        // compute Ndx and dNdy
        dNdx = dnpdx*std::pow(ndotn, -0.5f) + n * fnpx;
        dNdy = dnpdy*std::pow(ndotn, -0.5f) + n * fnpy;

        return n.normalize();
    }

    int get_clip_points(Ray& ray_, Diff_Geom ipoints[2], float t[2]) const{
        int numintersection = 0;
        Vector3D E = ray_.ori() - m_vertex;
        float AdD = m_axis.dot(ray_.dir());
        float cosSqr = m_costheta*m_costheta;
        float AdE = m_axis.dot(E);
        float DdE = ray_.dir().dot(E);
        float EdE = E.dot(E);
        float c2 = AdD*AdD-cosSqr;
        float c1 = AdD*AdE - cosSqr*DdE;
        float c0 = AdE*AdE - cosSqr*EdE;
        float dot;
        Vector3D zero;

        if (std::fabs(c2)>0) {
            float discr = c1*c1 - c0*c2;
            float invC2 = 1.f/c2;
            if (discr < 0) {
                // No intersection
                return  0;
            } else if (discr > 0) {
                // two distinct intersections
                float root = std::sqrt(discr);
                // entry point
                t[numintersection] = (-c1 + root) * invC2;
                E = ray_.at(t[numintersection]) - m_vertex;
                dot = E.dot(m_axis);
                if ( (dot > 0) && (dot <= m_height))
                {
                    fillConicalDiffGeom(ipoints[numintersection], ray_, t[numintersection], true);
                    ++numintersection;
                }
                // exit point
                t[numintersection] = (-c1 - root) * invC2;
                E = ray_.at(t[numintersection]) - m_vertex;
                dot = E.dot(m_axis);
                if ( (dot > 0)  && (dot <= m_height))
                {
                    fillConicalDiffGeom(ipoints[numintersection], ray_, t[numintersection], false);
                    ++numintersection;
                }
                return numintersection;
          } else {
                // one reapeated intersection : ray is tangent to the cone
                // may be return 0 instead of an intersection ?
                return 0;
                t[numintersection] = -c1 * invC2;
                E = ray_.at(t[numintersection]) - m_vertex;
                dot = E.dot(m_axis);
                if ( (dot > 0) && (dot <= m_height)) {
                    fillConicalDiffGeom(ipoints[numintersection], ray_, t[numintersection], true);
                    ++numintersection;
                }
                return numintersection;
           }
        } else if (std::fabs(c1) > 0) {
            // the ray is on the boundary of the cone
            // we consider no intersection
            // TODO : check this for CSG
            return 0;
        } else {
            //return false;
            // Cone contains ray V+tD
            // The ray intersect the cone exactly at its vertex :(
            // TODO : manage this particular case in another function
            ipoints[numintersection].set_pos(m_vertex);
            ipoints[numintersection].set_normal(-m_axis);

            ipoints[numintersection].set_u(1.f);
            ipoints[numintersection].set_v(0.f);

            E = ray_.ori() - m_vertex;
            t[numintersection] = ( (E.dot(ray_.dir())<0) ? std::sqrt(EdE) : -std::sqrt(EdE) ) ;
            ipoints[numintersection].set_t(t[numintersection]);
            ipoints[numintersection].set_in(true);
            ipoints[numintersection].set_u(0.f);
            ipoints[numintersection].set_v(1.f);

            // todo : compute here normal derivatives (according to x and y directions on the image plane)
            ipoints[numintersection].set_dNdx(zero);
            ipoints[numintersection].set_dNdy(zero);

            ++numintersection;
             // check with cap plane
            Plane cap(m_vertex + m_axis * m_height, m_axis);
            IntervalSet capset;
            if (cap.clip(ray_, capset)) {
                if (capset.bounds()[0].t < t[numintersection-1]) {
                    t[numintersection] = t[numintersection-1];
                    ipoints[numintersection] = ipoints[numintersection-1];
                    --numintersection;
                } else {
                    capset.bounds()[0].data->set_in(false);
                }
                ipoints[numintersection] = *(capset.bounds()[0].data);
                // TODO : update u, v dTdx and dTdy
                ipoints[numintersection].set_u(ipoints[numintersection].u()/(PSCALE*m_radius));
                ipoints[numintersection].set_v(ipoints[numintersection].v()/(PSCALE*m_radius));
                ipoints[numintersection].set_dTdx(ipoints[numintersection].dTdx()* (1.f/(PSCALE*m_radius)));
                ipoints[numintersection].set_dTdy(ipoints[numintersection].dTdy()* (1.f/(PSCALE*m_radius)));
                delete capset.bounds()[0].data;
                delete capset.bounds()[1].data;
                return 2;
            } else {
                // must never reach this point !
                assert(false);
                return 0;
            }
        }
    }

    Cone () : m_vertex(0.f,0.f, 1.f), m_axis(0.f, 0.f, -1.f), m_radius (1.f), m_height(1.f) {
        m_costheta = std::cos(std::atan2(m_radius, m_height));

    }

public:


    Cone (const Vector3D &center_, const Vector3D& vertex_, float radius_) : m_vertex(vertex_), m_radius (radius_) {
        m_axis = center_ - m_vertex;
        m_height = m_axis.norm();
        m_axis =   m_axis.normalize();
        m_costheta = std::cos(std::atan2(m_radius, m_height));
        W = m_axis;
        Vector3D::make_basis(U, V, W);
    }

    /**
       modifies the ray_.max_t() value, intersect only if the intersection
       is before ray_.max_t()
       Adapted from http://www.geometrictools.com/LibMathematics/Intersection/Wm5IntrLine3Cone3.cpp
    */
    bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const {
        Diff_Geom ipoints[2];
        float t[2];

        int numintersection = get_clip_points(ray_, ipoints,  t);
        /* numintersection == 2 -> two intersections with the cone above vertex :
         *          return the smallest t
         * numintersection == 1 -> one intersection with the cone above vertex :
         *          check cap plane (that will add an intersection point) before choosing the smalest positive t
         * numintersection == 0 -> no intersection
         */
        if (numintersection == 0)
            return false;
        else if (numintersection == 2) {
            int i=0;
            while ( (i<numintersection) && (t[i] < ray_.ray_epsilon())  )
                i++;
            if ( (i < numintersection) && (t[i] < ray_.max_t()) ) {
                *p_diff_geom_ = ipoints[i];
                ray_.set_max_t (t[i]);
                return true;
            } else
                return false;
        } else {
            // 1 intersection
            // check with cap plane
            Plane cap(m_vertex + m_axis * m_height, m_axis);
            Diff_Geom plane_intersection;
            Ray r(ray_);
            if (cap.intersect(ray_, &plane_intersection)) {
                if (ray_.max_t() < t[0]) {
                    // beware of in/out status
                    *p_diff_geom_ = plane_intersection;
                    float u, v;
                    u = p_diff_geom_->u()/(PSCALE*m_radius);
                    v = p_diff_geom_->v()/(PSCALE*m_radius);
                    p_diff_geom_->set_u(u);
                    p_diff_geom_->set_v(v);
                    Vector3D dTdx = p_diff_geom_->dTdx() * (1.f/(PSCALE*m_radius));
                    Vector3D dTdy = p_diff_geom_->dTdy() * (1.f/(PSCALE*m_radius));
                    p_diff_geom_->set_dTdx(dTdx);
                    p_diff_geom_->set_dTdy(dTdy);
                    return true;
                }
            }
            if ((t[0] > ray_.ray_epsilon()) && (t[0] <= ray_.max_t())) {
                // beware of in/out status
                *p_diff_geom_ = ipoints[0];
                ray_.set_max_t(t[0]);
                return true;
            } else
                return false;
        }
        return false;
    }

    bool clip(Ray& ray_, IntervalSet &bounds) const {
        Diff_Geom ipoints[2];
        float t[2];

        int numintersection = get_clip_points(ray_, ipoints,  t);
        if (numintersection == 0)
            return false;
        else if (numintersection == 2) {
            Diff_Geom *in = new Diff_Geom(ipoints[0].pos(), ipoints[0].normal(), ipoints[0].dNdx(), ipoints[0].dNdy(), t[0], ipoints[0].dPdx(), ipoints[0].dPdy(), ipoints[0].dDdx(), ipoints[0].dDdy(), ipoints[0].u(), ipoints[0].v(), ipoints[0].dTdx(), ipoints[0].dTdy(), true);
            Diff_Geom *out = new Diff_Geom(ipoints[1].pos(), ipoints[1].normal(), ipoints[1].dNdx(), ipoints[1].dNdy(), t[1], ipoints[1].dPdx(), ipoints[1].dPdy(), ipoints[1].dDdx(), ipoints[1].dDdy(), ipoints[1].u(), ipoints[1].v(), ipoints[1].dTdx(), ipoints[1].dTdy(), false);
            bounds.add(in, out);
            return true;
        } else {
            Diff_Geom *in, *out;
            IntervalSet capset;
            // check with cap plane
            Plane cap(m_vertex + m_axis * m_height, m_axis);
            if (cap.clip(ray_, capset) ) {
                in = capset.bounds()[0].data;
                in->set_u(in->u()/(PSCALE*m_radius));
                in->set_v(in->v()/(PSCALE*m_radius));
                in->set_dTdx(in->dTdx()* (1.f/(PSCALE*m_radius)));
                in->set_dTdy(in->dTdy()* (1.f/(PSCALE*m_radius)));
                if (in->t()< t[0]) {
                //if (p_diff_geom_in_->t() < t[0]) {
                    out = new Diff_Geom(ipoints[0].pos(), ipoints[0].normal(), ipoints[0].dNdx(), ipoints[0].dNdy(), t[0], ipoints[0].dPdx(), ipoints[0].dPdy(), ipoints[0].dDdx(), ipoints[0].dDdy(), ipoints[0].u(), ipoints[0].v(), ipoints[0].dTdx(), ipoints[0].dTdy(), false);
                } else {
                    out = in;
                    out->set_in(false);
                    in = new Diff_Geom(ipoints[0].pos(), ipoints[0].normal(), ipoints[0].dNdx(), ipoints[0].dNdy(), t[0], ipoints[0].dPdx(), ipoints[0].dPdy(), ipoints[0].dDdx(), ipoints[0].dDdy(), ipoints[0].u(), ipoints[0].v(), ipoints[0].dTdx(), ipoints[0].dTdy(), true);
                }
                delete (capset.bounds()[1].data);
                bounds.add(in, out);
                return true;
            } else {
                return false;
            }
        }
    }
};
