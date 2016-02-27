#pragma once

#include "ray.hpp"
#include "diff_geom.hpp"
#include "intervalset.hpp"

/** @ingroup Raytracer 
 *  Abstract class for geometric object
 */
class Geometry {
public:

    /**
       modifies the ray_.max_t() value, intersect only if the intersection
       is before ray_.max_t()
    */
    virtual bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const =0;

    /**
     * @brief clip a ray by a geometry.
     * @param ray_
     * @param p_diff_geom_in_ The point by wich the ray enters the geometry
     * @param p_diff_geom_out_ The point by wich the ray leaves the geometry
     * @return true if the ray intersects the geometry. In and out parameters are filled by this method.
     * The default returned value is false such that non volumic geometries do not need to implement this.
     *
     */
    virtual bool clip(Ray& ray_, IntervalSet &bounds) const =0;

    virtual void settangent(const Vector3D &tg){}
};
