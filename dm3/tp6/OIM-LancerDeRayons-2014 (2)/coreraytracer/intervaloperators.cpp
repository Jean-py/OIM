#include "intervaloperators.hpp"
#include <iostream>

extern "C" {

#include<stdio.h>

/*
 * Document explicatif : http://www.geometrictools.com/
 * Corrigé adapté de http://www.geometrictools.com/SampleMathematics/BooleanIntervalRectangle/IntervalSet.inl
 */

/** Calcule dans res l'union des deux ensemble d'intervalles a et b contenant respectivement size_a et size_b
bornes. Chaque borne est de type Bound et le champs data doit être manipulé de façon opaque.
Cette fonction doit remplir le tableau pré-alloué res. Cette pré-allocation assure que la mémoire sera
suffisante pour stocker le résultat de l'union de a et de b.
Cette fonction doit donner dans size_res le nombre exact de borne stockée dans res.
*/
void union_of_intervals(Bound *a, int size_a, Bound *b, int size_b, Bound *res, int *size_res){
    int nbout=0;
    int i0 = 0, i1 = 0, parity0 = 0, parity1 = 0;
    while (i0 < size_a && i1 < size_b) {
            float value0 = a[i0].t;
            float value1 = b[i1].t;

            if (value0 < value1) {
                if (parity0 == 0) {
                    parity0 = 1;
                    if (parity1 == 0) {
                        res[nbout++] = a[i0];
                    }
                } else {
                    if (parity1 == 0) {
                        res[nbout++] = a[i0];
                    }
                    parity0 = 0;
                }
                ++i0;
            } else if (value1 < value0) {
                if (parity1 == 0) {
                    parity1 = 1;
                    if (parity0 == 0) {
                        res[nbout++] = b[i1];
                    }
                } else {
                    if (parity0 == 0) {
                        res[nbout++] = b[i1];
                     }
                    parity1 = 0;
                }
                ++i1;
            } else { // value0 == value1
                if (parity0 == parity1) {
                    res[nbout++] = a[i0];
                }
                parity0 ^= 1;
                parity1 ^= 1;
                ++i0;
                ++i1;
            }
        }

    while (i0 < size_a) {
        res[nbout++] = a[i0];
        ++i0;
    }

    while (i1 < size_b) {
        res[nbout++] = b[i1];
        ++i1;
    }
    *size_res = nbout;
}

/** Calcule dans res l'intersection des deux ensemble d'intervalles a et b contenant respectivement size_a et size_b
bornes. Chaque borne est de type Bound et le champs data doit être manipulé de façon opaque.
Cette fonction doit remplir le tableau pré-alloué res. Cette pré-allocation assure que la mémoire sera
suffisante pour stocker le résultat de l'union de a et de b.
Cette fonction doit donner dans size_res le nombre exact de borne stockée dans res.
*/
void intersection_of_intervals(Bound *a, int size_a, Bound *b, int size_b, Bound *res, int *size_res){
    int nbout=0;
    int i0 = 0, i1 = 0, parity0 = 0, parity1 = 0;
    while (i0 < size_a && i1 < size_b) {
        float value0 = a[i0].t;
        float value1 = b[i1].t;

        if (value0 < value1) {
            if (parity0 == 0) {
                parity0 = 1;
                if (parity1 == 1) {
                    res[nbout++] = a[i0];
                }
            } else {
                if (parity1 == 1) {
                    res[nbout++] = a[i0];
                }
                parity0 = 0;
            }
            ++i0;
        } else if (value1 < value0) {
            if (parity1 == 0) {
                parity1 = 1;
                if (parity0 == 1) {
                    res[nbout++] = b[i1];
                }
            } else {
                if (parity0 == 1) {
                    res[nbout++] = b[i1];
                }
                parity1 = 0;
            }
            ++i1;
        } else { // value0 == value1
            if (parity0 == parity1) {
                res[nbout++] = a[i0];
            }
            parity0 ^= 1;
            parity1 ^= 1;
            ++i0;
            ++i1;
        }
    }
    *size_res = nbout;
}

/** Calcule dans res la différence des deux ensemble d'intervalles a et b contenant respectivement size_a et size_b
bornes. Chaque borne est de type Bound et le champs data doit être manipulé de façon opaque.
Cette fonction doit remplir le tableau pré-alloué res. Cette pré-allocation assure que la mémoire sera
suffisante pour stocker le résultat de l'union de a et de b.
Cette fonction doit donner dans size_res le nombre exact de borne stockée dans res.
*/
void difference_of_intervals(Bound *a, int size_a, Bound *b, int size_b, Bound *res, int *size_res){
    int nbout=0;
    int i0 = 0, i1 = 0, parity0 = 0, parity1 = 1;
    while (i0 < size_a && i1 < size_b)
    {
        float value0 = a[i0].t;
        float value1 = b[i1].t;

        if (value0 < value1) {
            if (parity0 == 0) {
                parity0 = 1;
                if (parity1 == 1) {
                    res[nbout++] = a[i0];
                }
            } else {
                if (parity1 == 1) {
                    res[nbout++] = a[i0];
                }
                parity0 = 0;
            }
            ++i0;
        } else if (value1 < value0) {
            if (parity1 == 0) {
                parity1 = 1;
                if (parity0 == 1) {
                    res[nbout++] = b[i1];
                }
            } else {
                if (parity0 == 1) {
                    res[nbout++] = b[i1];
                }
                parity1 = 0;
            }
            ++i1;
        } else { // value0 == value1
            if (parity0 == parity1)
            {
                res[nbout++] = a[i0];
            }
            parity0 ^= 1;
            parity1 ^= 1;
            ++i0;
            ++i1;
        }
    }

    while (i0 < size_a)
    {
        res[nbout++] = a[i0];
        ++i0;
    }
    *size_res = nbout;
}


}


