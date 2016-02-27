#pragma once
/** @defgroup Raytracer Simple Raytracer
 *  Raytracing library with simple functionalities
 */


#include "light.hpp"
#include "camera.hpp"
#include "primitive.hpp"

#include "ray.hpp"
#include "isect.hpp"

#include <vector>

using namespace std;


/** @ingroup Raytracer 
 *  Scene definition
 */
class Scene {
private:

	vector<Light *> m_p_lights;

	Camera * m_p_camera;

	vector<Primitive *> m_p_prims;

public:

	Scene () : m_p_camera (NULL) {
	}

	void set_camera (Camera * p_camera_) {
		m_p_camera = p_camera_;
	}

	void add_primitive (Primitive * p_prim_) {
		m_p_prims.push_back (p_prim_);
	}

	void add_light (Light * p_light_) {
		m_p_lights.push_back (p_light_);
	}

	const Camera * p_camera() const {
		return m_p_camera;
	}

	const vector<Light *>& p_lights () const {
		return m_p_lights;
	}

	const vector<Primitive *>& p_prims () const {
		return m_p_prims;
	}

	bool intersect (Ray& ray_, Isect * p_isect_) const {
		Isect isect;		
		bool has_isect = false;
		
		for (vector<Primitive *>::const_iterator it = m_p_prims.begin(); it != m_p_prims.end(); it++) {
			if ((*it)->intersect (ray_, &isect)) {
				has_isect = true;
				*p_isect_ = isect;
			}
		}

        if (has_isect){
            // update ray differentials
            ray_.setdPdx(p_isect_->dg().dPdx());
            ray_.setdPdy(p_isect_->dg().dPdy());
            ray_.setdDdx(p_isect_->dg().dDdx());
            ray_.setdDdy(p_isect_->dg().dDdy());

            float lod;
            BSDF bsdf = p_isect_->mat().evaluate (p_isect_->dg(), &lod);
            p_isect_->setBsdf(bsdf);
        }

		return has_isect;
	}

	/**
	   test the visibility between a point and a light
	   @return true if they are mutually visible
	 */
	bool test_visibility (const Vector3D& pos_, Light * p_light_) const {
        Vector3D dir = p_light_->pos() - pos_;
        float l = dir.norm();
        dir = dir.normalize();
        Ray ray (pos_+dir*0.001f, dir);
        ray.set_max_t(l);
		Isect isect;
		
		for (vector<Primitive *>::const_iterator it = m_p_prims.begin(); it != m_p_prims.end(); it++) {
			if ((*it)->intersect (ray, &isect)) {
				return false;
			}
		}
		return true;
	}
	
};

/**
 * \mainpage
 *
 * \section intro_sec Introduction
 * Cette documentation correspond à l'environnement de développement d'un lancer de rayon minimal utilisé 
 * pour les travaux pratiques de l'UE "Outils Informatiques pour le Multimédia" en Master 1 informatique de l'université Paul Sabatier, Toulouse.
 * 
 * Après réalisation de ce TP, vous devriez voir la scène par défaut suivante :
 *
 * \image html defaultScene.png
 * 
 * \section install_sec Installation
 * Télécharger l'archive <A HREF="../raytracing.tgz">raytracing.tgz</a> et la décompresser dans un répertoire spécifique.
 * Après décompression : les répertoires suivants seront générés :
 * \li \c main : Répertoire principal contenant le source à compléter/ecrire pour le TP (fichier main.c)
 * \li \c coreraytracer : Répertoire contenant l'architecture logicielle du lancer de rayons minimal
 * \li \c doc : Répertoire contenant cette documentation
 * 
 * Le code fourni n'a été testé que sous Linux et mac OS X et nécessite un compilateur C/C++.
 * 
 * \subsection dependances Dépendances
 * l'exploitation et la réalisation des travaux pratiques en 
 * utilisant ce code nécessitent l'installation des outils et de bibliothèque de développement pour gérer le format d'images <A HREF="http://www.openexr.com/">OpenEXR</a>.
 * Toutes les distributions linux proposent des paquets OpenEXR et les bibliothèques et outils sont aussi disponibles pour mac OS X et Windows. Nous ne détaillerons pas ici l'installation de ces outils et bibliothèques.
 * 
 * Il est recommandé d'installé l'outil <A HREF="http://qtpfsgui.sourceforge.net/">Luminance HDR</a> permettant de visualiser et de convertir interactivement les images HDR générées par
 * ce lancer de rayons minimal.
 * 
 * \subsection compilation Compilation
 * Le système de compilation utilisé par ce code est le système <A HREF="http://www.cmake.org/">cmake</a>.
 * L'ensemble des sources de ce logiciel de lancer de rayon minimal est composé de fichiers headers C++ définissant l'architecture de base et placés 
 * dans le répertoire \c coreraytracer . Le répertoire \c main contient le programme principal (qui devra être complété pendant les TPs) et un module 
 * d'interfaçage en C avec l'architecture de base. 
 * Toutefois, afin de simplifier l'utilisation de cet environnement, le compilateur à utiliser doit être un compilateur C++ même si <b>le code 
 * à écrire est du code C</b>.
 * 
 * Pour compiler le programme, nous utiliserons la possibilité offerte par cmake de compiler en dehors de l'arborescence des sources. 
 * Voici la marche à suivre pour compiler votre programme :
 *
 * \subsubsection compilation_premiere Première compilation
 * \li Créer (la première fois) un répertoire \c build dans le répertoire d'extraction de l'archive 
 * <A HREF="../raytracing.tgz">raytracing.tgz</a>
 * (au même niveau que les répertoires \c main et \c coreraytracer)
 * \li Dans un terminal, se positionner dans ce répertoire et taper la commande de génération des makefiles : <b>cmake ..</b>
 * \li Compiler le programme par la commande \b make
 
 * \subsubsection compilation_toutes Toutes les autres compilations
 * \li Compiler le programme par la commande \b make lancée dans le répertoire \c build
 
 * \subsection execution Exécution
 * \li Executer le programme en tappant la commande <b>./main</b> dans le répertoire \c bin
 * \li Une image ayant pour nom monimage.exr est enregistrée dans le répertoire d'exécution. Pour la visualiser, utiliser le programme
 * exrdisplay qui permet de régler l'exposition de l'image (la façon dont les couleurs haute dynamique dans le fichier .exr sont 
 * interprétée pour affichage à l'écran). La même image, transformée au format png par un algorithme de recalage de la dynamique des couleurs est aussi générée dans le même répertoire.
 *
 *
 * \section tp6 Sujet du TP6
 * Ce TP se décompose en trois étapes :
 * \li Calcul d'une image avec éclairage direct uniquement
 * \li Ajout des reflexions sépculaires (mirroir)
 * \li Ajout de la transparence
 *
 * \subsection partie1 Programmation du rendu d'une image sans réflexion ni transparence.
 * La première partie consiste à coder le corps des trois fonctions réalisant le lancer de rayon : compute_image(),  trace_ray() et compute_direct_lighting(). 
 * Ce sont les trois fonctions du fichier \b main.cpp. 
 * Pour vous aider dans votre programmation, vous trouverez une librarie de fonctions disponibles dans le fichier raytracing.h et documentées 
 * dans le module \ref RaytraceAPI
 *
 * Une fois ces fonctions codées, le programme calculera une image correspondant au rendu d'une scène composée de cinq sphères aux propriétés optiques
 * différentes (plastique, métal, verre, platre) posées sur un plan. 
 * Dans cette première version, seul les rayons primaires (rayons lancés depuis la caméra) sont pris en compte. 
 * Aucune réflexion ou transparence ne sera calculée dans cette première partie.
 *
 * Pour commencer, vous allez coder la fonction compute_image() qui est celle, comme son nom l'indique, qui effectue le calcul d'une image pixel par pixel. 
 * L'algorithme de cette fonction a été donné dans le <A HREF="http://www.irit.fr/~Mathias.Paulin/M1Info/OIM/OIM-Cours5.pdf">cours 5</a>.
 * Nous décrivons ici le principe de cette fonction qui devra être programmée en utilisant les outils fournis par les 
 * modules \ref RayAPI et \ref ImageAPI
 *
 * Le calcul d'une image de synthèse consiste à calculer une couleur pour tous les pixels d'une image en fonction d'une modélisation d'un 
 * capteur virtuel.
 * Ce capteur est ici représenté par une Camera qui permet, à partir des coordonnées <b>(x,y)</b> d'un pixel d'une image, de créer le rayon 
 * passant par ce pixel.
 * La manière dont est créé ce rayon dépend de la Camera. La scène étant crée avec une caméra par défaut de type \b pinhole, la fonction 
 * camera_ray() devra être utilisée afin de créer le rayon primaire. Ce rayon est ensuite tracé dans la scène (par la fonction trace_ray() 
 * à écrire dans la seconde étape de ce TP) et la couleur calculée à l'aide de ce rayon doit être stockée sur le pixel <b>(x,y)</b>. 
 * Voir le module \ref ImageAPI pour réaliser cette action.
 *
 * Votre fonction devra donc, pour chaque pixel de l'image (créée avec la scène dans la fonction main()), dont on prendra soin de récupérer la 
 * résolution, créer le rayon correspondant, le tracer dans la scène et stocker la couleur résultante dans l'image.
 *
 * A noter : tant que les deux autres fonctions ne sont pas programmée, aucune image ne sera réellement calculée.
 *
 * Dans un second temps vous allez coder la fonction trace_ray() qui lance un rayon et retourne une couleur obtenue par la somme de 
 * l'éclairage direct (couleur calculée par la fonction compute_direct_lighting()) et des couleurs provenant des reflets et transparences
 * éventuels aux points d'intersection. Ces deux dernières informations seront calculées dans la deuxième partie de ce TP.
 * Comme nous nous intéressons ici qu'aux rayons primaires, cette fonction ne sera pas récursive puisque seules les premières intersections 
 * avec la scène sont nécessaires pour calculer la couleur de chaque pixel. Cette couleur correspond à l'éclairage direct des points visibles 
 * depuis la caméra.
 *
 * Comme vu dans le <A HREF="http://www.irit.fr/~Mathias.Paulin/M1Info/OIM/OIM-Cours1.pdf">cours 1</a>, l'espace de couleur utilisé pour le calcul d'une image 
 * est l'espace \b RGB. Cet espace est additif, c'est à dire qu'une couleur complexe peut être obtenue en faisant la somme de couleurs simples.
 * De plus, le phénomène de réflexion de la lumière à la surface d'un objet coloré se modélise par une multiplication entre la couleur de 
 * l'objet (son aspect) et la couleur de la lumière incidente.
 * Une couleur est représentée, dans le coeur de notre lancé de rayons par un objet de type Color avec les opérations décrites dans le 
 * module \ref ColorAPI.
 *
 * Lorsque l'on calcule l'intersection entre un rayon et la scène (fonction intersect_scene() du module \ref RayAPI), une structure 
 * de type Isect est construite contenant les informations sur cette intersection (point, normale, aspect).
 *
 * Dans cette première partie du TP, la fonction trace_ray() ne calculant que les rayons primaires, nous devons donc calculer l'intersection 
 * entre le rayon et la scène.
 * S'il n'y a pas d'intersection, une couleur blanche (triplet \b RGB [1, 1, 1], élément neutre de la multiplication des couleurs) 
 * devra être retournée.
 * S'il y a une intersection, la couleur retournée sera la couleur résultante de l'éclairage direct du point d'intersection par les 
 * sources lumineuses de la scène et calculée par la fonction compute_direct_lighting() à écrire ensuite.
 *
 * Dans un troisième et dernier temps pour cette première partie, nous devons calculer l'éclairage direct en un point de la scène. 
 * Ce calcul correspond à la fonction compute_direct_lighting().
 * Le calcul de l'éclairage direct dépend d'une part du point de la scène pour lequel il faut réaliser l'opération (décrit par une 
 * structure Isect) et d'autre part de la direction de vue de ce point (décrit par une structure de type Ray).
 *
 * L'éclairage direct en un point depuis une source lumineuse est fonction de la visibilité dans l'espace entre ces deux points. Si la source 
 * n'est pas visibles, le point est dans l'ombre et la couleur de la source n'est donc pas ajoutée à la couleur du point d'intersection.
 *
 * Le rôle de la fonction compute_direct_lighting() est de calculer la somme, pour chaque source de lumière de la scène, de l'éclairage
 * élémentaire dû à cette source. Cette somme est calculée en ajoutant à une couleur noire (triplet \b RGB [0, 0, 0], élément neutre de 
 * l'addition des couleurs), la valeur de l'éclairage direct lié à chaque source de la scène. La fonction compute_direct_lighting()
 * utilise pour cela les fonctions des modules \ref SceneAPI (pour accéder à l'ensemble des sources lumineuses de la scène), 
 * \ref RayAPI (pour calculer la visibilité entre un point et une source de lumière) et \ref LightAPI (pour calculer l'éclairage direct).
 *
 *
 * \subsection partie2 Programmation du calcul des reflexions.
 *
 * Cette seconde partie consiste à ajouter les réflexions spéculaire par une programmation récursive de la fonction trace_ray().
 * Afin de déterminer un cas d'arrêt pour les appels récursif, deux solutions sont utilisables. Chaque appel récursif de la 
 * fonction trace_ray() rajoute un niveau de réflexion. Une première possibilité pour arrêter la récursion est de limiter le 
 * nombre de réflexion à une valeur arbitraire en fonction de la scène et de la précision voulue dans le calcul de l'image (par exemple 10).
 * Afin de permettre ce cas d'arêt, chaque rayon est étiquetté par sa profondeur, correspondant au nombre de réflexion qu'il a eu dans la scène.
 *
 * Une deuxième manière de terminer la recursivité repose sur l'importance d'un rayon pour un pixel. Lors d'une réflexion spéculaire, la 
 * couleur transportée par un rayon est multipliée par un coefficient de réflexion inférieur à 1 pour des raisons physiques de conservation de l'énergie.
 * Ainsi, après de nombreux rebonds, la couleur d'un rayon est multipliée par un coefficient très faible et n'a plus d'impact sur le pixel. 
 * On peut alors terminer la récursivité sans perte de qualité dans l'image.
 * Afin de permettre ce cas d'arrêt, le rayon est étiqueté par son importance.
 *
 * A partir des fonctions définies dans le module \ref RayAPI et permettant d'accéder à ces informations sur le rayon, définir un cas d'arêt de
 * la récursivité et renvoyer si ce cas est vérifié la couleur résultante de l'éclairage direct.
 *
 * Ensuite, si la récursivité n'est pas terminée, en utilisant les fonctions définies dans le module \ref LightAPI, calculer la couleur réfléchie.
 * Pour cela, il  faut tester si le matériau est réflechissant et, si c'est le cas, calculer le rayon réfléchi et le coefficient de 
 * réflexion (une couleur). La couleur calculée en lançant le rayon réfléchi devra alors être multipliée par ce coefficient avant d'être ajoutée
 * à la couleur renvoyée par trace_ray().
 *
 * \subsection partie3 Programmation du calcul de la transparence.
 *
 * De façon très similaire à la réflexion, utiliser les fonctions définies dans le module \ref LightAPI pour calculer la couleur réfractée.
 * Pour cela, il  faut tester si le matériau est transparent et, si c'est le cas, calculer le rayon réfracté et le coefficient de 
 * transparence (une couleur). La couleur calculée en lançant le rayon réfracté devra alors être multipliée par ce coefficient avant
 * d'être ajoutée à la couleur renvoyée par trace_ray().
 *
 */

