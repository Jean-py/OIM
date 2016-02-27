#pragma once

#include "diff_geom.hpp"

extern "C" {

    /** Une borne est défini par sa coordonnée t et une donnée opaque data.
     Un intervalle est défini par deux bornes a et b de telle sorte que a.t < b.t
    */
    typedef struct Bound_{
        float t;
        Diff_Geom *data;
    } Bound;

    /** Un ensemble d'intervalle est un tableau de bornes contenant n éléments permettant de définir n/2 intervalles
    Les fonctions suivantes doivent être écrite par les étudiants en respectant impérativement leur spécification
    afin de pouvoir être utilisées par le moteur de lancer de rayon.
    Il n'y a aucune autre contrainte sur l'écriture de ces fonctions.
    */

    /** Calcule dans res l'union des deux ensemble d'intervalles a et b contenant respectivement size_a et size_b
    bornes. Chaque borne est de type Bound et le champs data doit être manipulé de façon opaque.
    Cette fonction doit remplir le tableau pré-alloué res. Cette pré-allocation assure que la mémoire sera
    suffisante pour stocker le résultat de l'union de a et de b.
    Cette fonction doit donner dans size_res le nombre exact de borne stockée dans res.
    */
    extern void union_of_intervals(Bound *a, int size_a, Bound *b, int size_b, Bound *res, int *size_res);

    /** Calcule dans res l'intersection des deux ensemble d'intervalles a et b contenant respectivement size_a et size_b
    bornes. Chaque borne est de type Bound et le champs data doit être manipulé de façon opaque.
    Cette fonction doit remplir le tableau pré-alloué res. Cette pré-allocation assure que la mémoire sera
    suffisante pour stocker le résultat de l'union de a et de b.
    Cette fonction doit donner dans size_res le nombre exact de borne stockée dans res.
    */
    extern void intersection_of_intervals(Bound *a, int size_a, Bound *b, int size_b, Bound *res, int *size_res);

    /** Calcule dans res la différence des deux ensemble d'intervalles a et b contenant respectivement size_a et size_b
    bornes. Chaque borne est de type Bound et le champs data doit être manipulé de façon opaque.
    Cette fonction doit remplir le tableau pré-alloué res. Cette pré-allocation assure que la mémoire sera
    suffisante pour stocker le résultat de l'union de a et de b.
    Cette fonction doit donner dans size_res le nombre exact de borne stockée dans res.
    */
    extern void difference_of_intervals(Bound *a, int size_a, Bound *b, int size_b, Bound *res, int *size_res);

};

