#pragma once

#include "geometry.hpp"
#include "plane.hpp"

/** @ingroup Raytracer 
 *  Cylinder geometric object
 */

// TODO : compute ray differentials !!!

class Cylinder : public Geometry {
private:

    Vector3D m_base;
    Vector3D m_axis;
    float m_radius;
    float m_length;
    Vector3D U, V, W;


    void computePlanarTexInfo(const Vector3D &pt_inter, const Vector3D &normal, const Vector3D &dPpdx, const Vector3D &dPpdy, float &u, float &v, Vector3D &dTdx, Vector3D &dTdy) const{
        Vector3D diff = pt_inter - m_base;
        Vector3D ptOnCylinder(U.dot(diff), V.dot(diff), W.dot(diff));
        u = ((ptOnCylinder[0]+m_radius)*0.5/(4.*m_radius));
        v = ((ptOnCylinder[1]+m_radius)*0.5/(4.*m_radius));
        // compute texture differential
        float coef = (1.f / (4.f * m_radius));
        dTdx[0] = coef * U.dot(dPpdx);
        dTdx[1] = coef * V.dot(dPpdx);
        dTdy[0] = coef * U.dot(dPpdy);
        dTdy[1] = coef * V.dot(dPpdy);
    }

    void fillPlanarDiffGeom(Diff_Geom &dg, const Ray &r, float t, const Vector3D &normal  , bool in) const{
        Vector3D pt_inter = r.at(t);
        Vector3D zero; // normal derivatives are zero on plane
        Vector3D dPpdx, dPpdy;
        r.propagateDifferentials(t, normal, dPpdx,dPpdy);
         // texture coordinate
        Vector3D dTdx;
        Vector3D dTdy;
        float u, v;
        computePlanarTexInfo(pt_inter, normal, dPpdx, dPpdy, u, v, dTdx, dTdy);
        dg = Diff_Geom(pt_inter, normal, zero, zero, t, dPpdx, dPpdy, r.dDdx(), r.dDdy(), u, v, dTdx, dTdy, in);
    }

    void computeCylindricalTexInfo(const Vector3D &pt_inter, const Vector3D &normal, const Vector3D &dPpdx, const Vector3D &dPpdy, float &u, float &v, Vector3D &dTdx, Vector3D &dTdy) const{
        Vector3D diff = pt_inter - m_base;
        Vector3D ptOnCylinder(U.dot(diff), V.dot(diff), W.dot(diff));
        ptOnCylinder[0] = ptOnCylinder[0]/m_radius;
        ptOnCylinder[1] = ptOnCylinder[1]/m_radius;
        // texture coordinates
        u = (std::atan2(ptOnCylinder[1],ptOnCylinder[0])+M_PI)/(2.f*M_PI);
        v = ((ptOnCylinder[2]+m_length)*0.5/m_length);
        // compute texture differential
        dTdx[0] = (1.f/(2.f*M_PI)) * (1.f/(1.f + std::pow(ptOnCylinder[1]/ptOnCylinder[0], 2.f)))*( (V.dot(dPpdx)*ptOnCylinder[0] - U.dot(dPpdx)*ptOnCylinder[1]) / std::pow(ptOnCylinder[0], 2.f) );
        dTdy[0] =  (1.f/(2.f*M_PI)) * (1.f/(1.f + std::pow(ptOnCylinder[1]/ptOnCylinder[0], 2.f)))*( (V.dot(dPpdy)*ptOnCylinder[0] - U.dot(dPpdy)*ptOnCylinder[1]) /std::pow(ptOnCylinder[0], 2.f) );
        float coef = (1.f / (2.f*m_length));
        dTdx[1] = coef * W.dot(dPpdx);
        dTdy[1] = coef * W.dot(dPpdy);
    }

    void fillCylindricalDiffGeom(Diff_Geom &dg, const Ray &r, float t, bool in) const{
        Vector3D pt_inter = r.at(t);
        Vector3D dPpdx, dPpdy;
		
        Vector3D cp = pt_inter - m_base;
        Vector3D ap = m_axis.cross_product(cp);
        Vector3D np = ap.cross_product(m_axis);
        float npdotnp = np.dot(np);
        Vector3D normal = np.normalize();
        r.propagateDifferentials(t, normal, dPpdx, dPpdy);

        Vector3D dnpdx = (m_axis.cross_product(dPpdx)).cross_product(m_axis);
        Vector3D dnpdy = (m_axis.cross_product(dPpdy)).cross_product(m_axis);
        float fnpx = -std::pow(npdotnp, -1.5f) * dnpdx.dot(np);
        float fnpy = -std::pow(npdotnp, -1.5f) * dnpdy.dot(np);
        // compute Ndx and dNdy
        Vector3D dNdx, dNdy;
        dNdx = dnpdx*std::pow(npdotnp, -0.5f) + np * fnpx;
        dNdy = dnpdy*std::pow(npdotnp, -0.5f) + np * fnpy;
        // compute texinfo
        Vector3D dTdx;
        Vector3D dTdy;
        float u, v;
        computeCylindricalTexInfo(pt_inter, normal, dPpdx, dPpdy, u, v, dTdx, dTdy);

        dg = Diff_Geom(pt_inter, normal, dNdx, dNdy, t, dPpdx, dPpdy, r.dDdx(), r.dDdy(), u, v, dTdx, dTdy, in);
    }

    int get_clip_points(Ray& ray_, Diff_Geom ipoints[2], float t[2]) const{
        Vector3D zero;
        float rSqr = m_radius*m_radius;

        // convert incoming line origin to cylinder coordinates
        Vector3D diff = ray_.ori() - m_base;
        Vector3D P(U.dot(diff), V.dot(diff), W.dot(diff));
        // Get the z-value, in cylinder coordinates, of the incoming line's
        // unit-length direction.
        float dz = W.dot(ray_.dir());

        if (std::fabs(dz) >= 1.f - ray_.ray_epsilon()) {
            // The line is parallel to the cylinder axis.  Determine if the line
            // intersects the cylinder end disks.
            float radialSqrDist = rSqr - P.x()*P.x() - P.y()*P.y();
            if (radialSqrDist < 0.f) { // epsilon ?
                // Line outside the cylinder, no intersection.
                return 0; // 0
            }
            // Line intersects the cylinder end disks.
            // todo verify normals
            if (dz > 0.f) {
                t[0] = -P.z() - m_length;
                fillPlanarDiffGeom(ipoints[0], ray_, t[0], m_axis, true);
                t[1] = -P.z() + m_length;    
                fillPlanarDiffGeom(ipoints[1], ray_, t[1], -m_axis, false);

            } else {
                t[0] = P.z() - m_length;
                fillPlanarDiffGeom(ipoints[0], ray_, t[0], -m_axis, true);
                t[1] = P.z() + m_length;
                fillPlanarDiffGeom(ipoints[1], ray_, t[1], m_axis, false);
            }
            return 2;
        }

        // convert incoming line unit-length direction to cylinder coordinates
        Vector3D D(U.dot(ray_.dir()),V.dot(ray_.dir()),dz);
        float a0, a1, a2, discr, root, inv, tValue;
        if (std::fabs(D.z()) <= ray_.ray_epsilon()) {
            // The line is perpendicular to the cylinder axis.
            if (std::fabs(P.z()) > m_length) {
                // Line is outside the planes of the cylinder end disks.
                return 0;
            }
            // Test intersection of line P+t*D with infinite cylinder
            // x^2+y^2 = r^2.  This reduces to computing the roots of a
            // quadratic equation.  If P = (px,py,pz) and D = (dx,dy,dz),
            // then the quadratic equation is
            //   (dx^2+dy^2)*t^2 + 2*(px*dx+py*dy)*t + (px^2+py^2-r^2) = 0
            a0 = P.x()*P.x() + P.y()*P.y() - rSqr;
            a1 = P.x()*D.x() + P.y()*D.y();
            a2 = D.x()*D.x() + D.y()*D.y();
            discr = a1*a1 - a0*a2;
            if (discr < 0.f) {
                // Line does not intersect cylinder.
                return 0;
            }
            else if (discr > 0) { // epsilon ?
                // Line intersects cylinder in two places.
                root = std::sqrt(discr);
                inv = 1.f/a2;
                t[0] = (-a1 - root)*inv;
                fillCylindricalDiffGeom(ipoints[0], ray_, t[0], true);
                t[1] = (-a1 + root)*inv;
                fillCylindricalDiffGeom(ipoints[1], ray_, t[1], false);
                return 2;
            }
            else
            {
                // Line is tangent to the cylinder.
                //t[0] = -a1/a2;
                //return 1;
                return 0;
            }
        }
        // Test plane intersections first.
        int quantity = 0;
        inv = 1.0f/D.z();

        float t0 = (-m_length - P.z())*inv;
        float xTmp = P.x() + t0*D.x();
        float yTmp = P.y() + t0*D.y();
        if (xTmp*xTmp + yTmp*yTmp <= rSqr) {
            // Planar intersection inside the top cylinder end disk.
            t[quantity] = t0;
            fillPlanarDiffGeom(ipoints[quantity], ray_, t[quantity], m_axis, true);
            ++quantity;
        }

        float t1 = (+m_length - P.z())*inv;
        xTmp = P.x() + t1*D.x();
        yTmp = P.y() + t1*D.y();
        if (xTmp*xTmp + yTmp*yTmp <= rSqr) {
            // Planar intersection inside the bottom cylinder end disk.
            t[quantity] = t1;
            fillPlanarDiffGeom(ipoints[quantity], ray_, t[quantity], -m_axis, false);
            ++quantity;            
        }

        if (quantity == 2) {
            // Line intersects both top and bottom cylinder end disks.
            if (t[0] > t[1]) {
                float save = t[0];
                t[0] = t[1];
                t[1] = save;
                Diff_Geom gds = ipoints[0];
                ipoints[0] = ipoints[1];
                ipoints[1] = gds;
                ipoints[0].set_in(true);
                ipoints[1].set_in(false);
            }
            return 2;
        }
        // If quantity == 1, then the line must intersect cylinder wall in a
        // single point somewhere between the end disks.  This case is detected
        // in the following code that tests for intersection between line and
        // cylinder wall.
        a0 = P.x()*P.x() + P.y()*P.y() - rSqr;
        a1 = P.x()*D.x() + P.y()*D.y();
        a2 = D.x()*D.x() + D.y()*D.y();
        discr = a1*a1 - a0*a2;
        if (discr < 0.f) {
            // Line does not intersect cylinder wall.
            //assertion(quantity == 0, "Unexpected condition\n");
            return 0;
        } else if (discr > 0.f) {
            root = std::sqrt(discr);
            inv = 1.f/a2;
            tValue = (-a1 - root)*inv;
            if (t0 <= t1) {
                if (t0 <= tValue && tValue <= t1) {
                    t[quantity] = tValue;
                    fillCylindricalDiffGeom(ipoints[quantity], ray_, t[quantity], false);
                    ++quantity;
                }
            } else {
                if (t1 <= tValue && tValue <= t0) {
                    t[quantity] = tValue;
                    fillCylindricalDiffGeom(ipoints[quantity], ray_, t[quantity], false);
                    ++quantity;
                }
            }
            if (quantity == 2) {
                // Line intersects one of the cylinder end disks and once on the
                // cylinder wall.
                if (t[0] > t[1]) {
                    float save = t[0];
                    t[0] = t[1];
                    t[1] = save;
                    Diff_Geom gds = ipoints[0];
                    ipoints[0] = ipoints[1];
                    ipoints[1] = gds;
                    ipoints[0].set_in(true);
                    ipoints[1].set_in(false);
                }

                return 2;
            }
            tValue = (-a1 + root)*inv;
            if (t0 <= t1) {
                if (t0 <= tValue && tValue <= t1) {
                    t[quantity] = tValue;
                    fillCylindricalDiffGeom(ipoints[quantity], ray_, t[quantity], false);
                    ++quantity;
                }
            } else {
                if (t1 <= tValue && tValue <= t0) {
                    t[quantity] = tValue;
                    fillCylindricalDiffGeom(ipoints[quantity], ray_, t[quantity], false);
                    ++quantity;
                }
            }
        } else {
            tValue = -a1/a2;
            if (t0 <= t1) {
                if (t0 <= tValue && tValue <= t1) {
                    t[quantity] = tValue;
                    fillCylindricalDiffGeom(ipoints[quantity], ray_, t[quantity], false);
                    ++quantity;
                }
            } else {
                if (t1 <= tValue && tValue <= t0) {
                    t[quantity] = tValue;
                    fillCylindricalDiffGeom(ipoints[quantity], ray_, t[quantity], false);
                    ++quantity;
                }
            }
        }
        if (quantity == 2) {
            if (t[0] > t[1]) {
                float save = t[0];
                t[0] = t[1];
                t[1] = save;
                Diff_Geom gds = ipoints[0];
                ipoints[0] = ipoints[1];
                ipoints[1] = gds;
                ipoints[0].set_in(true);
                ipoints[1].set_in(false);
            }
        }
        return quantity;
    }

    Cylinder () : m_radius (0.f) {
        W = m_axis;
        Vector3D::make_basis(U, V, W);
    }

public:


    Cylinder (const Vector3D& center_, const Vector3D& axis_, float radius_, float length_) : m_base (center_), m_axis(axis_.normalize()), m_radius (radius_), m_length(length_/2.f) {
            W = m_axis;
            Vector3D::make_basis(U, V, W);
	}

	/**
	   modifies the ray_.max_t() value, intersect only if the intersection
	   is before ray_.max_t()
       adapted from http://www.geometrictools.com/LibMathematics/Intersection/Wm5IntrLine3Cylinder3.cpp
	*/
	bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const {
        Diff_Geom ipoints[2];
        float t[2];
        int numintersection = get_clip_points(ray_, ipoints,  t);
        if (numintersection == 0)
            return false;
        else if (numintersection == 2) {
            int i;
            for (i=0; (i<numintersection) && (t[i] < ray_.ray_epsilon()); i++);
            if ( (i < numintersection) && (t[i] < ray_.max_t()) ) {
                *p_diff_geom_ = ipoints[i];
                ray_.set_max_t (t[i]);
                return true;
            } else
                return false;
        } else
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
            return false;
        }
    }
};
