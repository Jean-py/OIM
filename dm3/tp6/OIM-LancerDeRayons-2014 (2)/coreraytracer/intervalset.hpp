#pragma once

#include "intervaloperators.hpp"
#include <vector>
#include <iostream>

// Open interval set
// adapted from http://www.geometrictools.com/SampleMathematics/BooleanIntervalRectangle/IntervalSet.h

class IntervalSet {
    std::vector<Bound> m_bounds;
public:
    IntervalSet(){}

    void add(Diff_Geom* in, Diff_Geom* out){
        Bound bound_in;
        bound_in.t = in->t();
        bound_in.data=in;
        Bound bound_out;
        bound_out.t = out->t();
        bound_out.data=out;
        m_bounds.push_back(bound_in);
        m_bounds.push_back(bound_out);
    }

    std::vector<Bound> &bounds(){
        return m_bounds;
    }

    void union_operator(const IntervalSet &other, IntervalSet &res){
        int size_res=0;
        res.m_bounds.resize(m_bounds.size() + other.m_bounds.size());
        union_of_intervals(const_cast<Bound *>(&(m_bounds[0])), m_bounds.size(), const_cast<Bound *>(&(other.m_bounds[0])), other.m_bounds.size(), &(res.m_bounds[0]), &size_res);
        res.m_bounds.resize(size_res);
    }

    void intersection_operator(const IntervalSet &other, IntervalSet &res){
        int size_res=0;
        res.m_bounds.resize(std::max(m_bounds.size(), other.m_bounds.size()));
        intersection_of_intervals(const_cast<Bound *>(&(m_bounds[0])), m_bounds.size(), const_cast<Bound *>(&(other.m_bounds[0])), other.m_bounds.size(), &(res.m_bounds[0]), &size_res);
        res.m_bounds.resize(size_res);
    }

    void difference_operator(const IntervalSet &other, IntervalSet &res){
        int size_res=0;
        res.m_bounds.resize(m_bounds.size() + other.m_bounds.size());
        difference_of_intervals(const_cast<Bound *>(&(m_bounds[0])), m_bounds.size(), const_cast<Bound *>(&(other.m_bounds[0])), other.m_bounds.size(), &(res.m_bounds[0]), &size_res);
        res.m_bounds.resize(size_res);
    }

};
