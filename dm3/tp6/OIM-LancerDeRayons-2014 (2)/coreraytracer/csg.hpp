#pragma once

// http://www.cse.ohio-state.edu/~parent/classes/681/Lectures/19.RayTracingCSG.pdf

/**
  Syntaxe d'un arbre CSG :
  Opérateurs terminaux
  Primitive :
    Sphére = (sp pos_x pos_y pos_z radius)
            -- sphére définie par son centre et son rayon
    Cylindre = (cy pos_x pos_y pos_z axis_x axis_y axis_z radius length)
            -- cylindre définit par son centre son axe, son rayon et sa longueur
    Cône = (co pos_x pos_y pos_z vertex_x vertex_y vertex_z radius)
            -- cone définit par le centre de sa base, son sommet et le rayon de sa base
    Box = (bo pos_x pos_y pos_z baseX_x baseX_y baseX_z baseY_x baseY_y baseY_z size_x size_y size_z)
            -- boite définie par son centre, deux vecteurs définissant son plan de base et les longueurs des aretes
  Operateurs :
    Union        : (+ Operateur|Primitive Operateur|Primitive)
    Intersection : (* Operateur|Primitive Operateur|Primitive)
    Différence   : (- Operateur|Primitive Operateur|Primitive)




    1) Ecrire un analyseur syntaxique qui transforme une chaine de caractère définissant un objet CSG en un arbre CSG

        (- (* (cu ... ) (cu ... )) (+ (sp ...) (cy ...)) )
                diff
        Inter           Union
    cube     cube   sphere  cylindre



*/

#include "geometry.hpp"
#include "intervalset.hpp"
#include <cmath>
#include <cassert>
#include <vector>

class CSGIntersection : public Geometry {
private:
    Geometry *m_a;
    Geometry *m_b;
public:
    CSGIntersection(Geometry *a, Geometry *b):m_a(a), m_b(b) {

    }
    /**
       modifies the ray_.max_t() value, intersect only if the intersection
       is before ray_.max_t()
    */
    bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const {
        IntervalSet bounds;
        if (clip(ray_, bounds)) {
            unsigned int i;
            std::vector<Bound> &allbounds = bounds.bounds();
            for (i=0; i<allbounds.size(); ++i){
                if (allbounds[i].t >= ray_.ray_epsilon())
                    break;
            }
            if (i<allbounds.size()){
                Diff_Geom *inter_p = allbounds[i].data;
                if (  (inter_p->t() < ray_.max_t()) ){
                    ray_.set_max_t(inter_p->t());
                    *p_diff_geom_ = *inter_p;
                    return true;
                } else
                    return false;
            } else
                return false;

        } else {
            return false;
        }
    }

    /**
     * @brief clip a ray by a geometry.
     * @param ray_
     * @param p_diff_geom_in_ The point by wich the ray enters the geometry
     * @param p_diff_geom_out_ The point by wich the ray leaves the geometry
     * @return true if the ray intersects the geometry. In and out parameters are filled by this method.
     * The default returned value is false such that non volumic geometries do not need to implement this.
     *
     */
    bool clip(Ray& ray_, IntervalSet &bounds) const {
        IntervalSet set_a, set_b;
        if (m_a->clip(ray_, set_a)) {
            if (m_b->clip(ray_, set_b)){
                set_a.intersection_operator(set_b, bounds);
                return (bounds.bounds().size()>0);
            }
        }
        return false;
    }


};

class CSGUnion : public Geometry {
private:
    Geometry *m_a;
    Geometry *m_b;
public:
    CSGUnion(Geometry *a, Geometry *b):m_a(a), m_b(b) {

    }
    /**
       modifies the ray_.max_t() value, intersect only if the intersection
       is before ray_.max_t()
    */
    bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const {
        IntervalSet bounds;
        if (clip(ray_, bounds)) {
            unsigned int i;
            std::vector<Bound> &allbounds = bounds.bounds();
            for (i=0; i<allbounds.size(); ++i){
                if (allbounds[i].t >= ray_.ray_epsilon())
                    break;
            }
            if (i<allbounds.size()){
                Diff_Geom *inter_p = allbounds[i].data;
                if (  (inter_p->t() < ray_.max_t()) ){
                    ray_.set_max_t(inter_p->t());
                    *p_diff_geom_ = *inter_p;
                    return true;
                } else
                    return false;
            } else
                return false;

        } else {
            return false;
        }
    }

    /**
     * @brief clip a ray by a geometry.
     * @param ray_
     * @param p_diff_geom_in_ The point by wich the ray enters the geometry
     * @param p_diff_geom_out_ The point by wich the ray leaves the geometry
     * @return true if the ray intersects the geometry. In and out parameters are filled by this method.
     * The default returned value is false such that non volumic geometries do not need to implement this.
     *
     */
    bool clip(Ray& ray_, IntervalSet &bounds) const {
        IntervalSet set_a, set_b;
        m_a->clip(ray_, set_a);
        m_b->clip(ray_, set_b);
        set_a.union_operator(set_b, bounds);
        return (bounds.bounds().size()>0);
     }


};

class CSGDifference : public Geometry {
private:
    Geometry *m_a;
    Geometry *m_b;
public:
    CSGDifference(Geometry *a, Geometry *b):m_a(a), m_b(b) {

    }
    /**
       modifies the ray_.max_t() value, intersect only if the intersection
       is before ray_.max_t()
    */
    bool intersect (Ray& ray_, Diff_Geom * p_diff_geom_) const {
        IntervalSet bounds;
        if (clip(ray_, bounds)) {
            unsigned int i;
            std::vector<Bound> &allbounds = bounds.bounds();
            for (i=0; i<allbounds.size(); ++i){
                if (allbounds[i].t >= ray_.ray_epsilon())
                    break;
            }
            if (i<allbounds.size()){
                Diff_Geom *inter_p = allbounds[i].data;
                if (  (inter_p->t() < ray_.max_t()) ){
                    ray_.set_max_t(inter_p->t());
                    *p_diff_geom_ = *inter_p;
                    return true;
                } else
                    return false;
            } else
                return false;

        } else {
            return false;
        }
    }

    /**
     * @brief clip a ray by a geometry.
     * @param ray_
     * @param p_diff_geom_in_ The point by wich the ray enters the geometry
     * @param p_diff_geom_out_ The point by wich the ray leaves the geometry
     * @return true if the ray intersects the geometry. In and out parameters are filled by this method.
     * The default returned value is false such that non volumic geometries do not need to implement this.
     *
     */
    bool clip(Ray& ray_, IntervalSet &bounds) const {
        IntervalSet set_a, set_b;
        m_a->clip(ray_, set_a);
        m_b->clip(ray_, set_b);
        set_a.difference_operator(set_b, bounds);
        return (bounds.bounds().size()>0);
    }


};
