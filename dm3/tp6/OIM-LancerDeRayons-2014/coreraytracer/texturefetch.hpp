#pragma once

#include "color.hpp"
#include <cmath>

//extern "C" {

/**
  @page HomeworkAssignment DEVOIR : Opérateurs pour l'accès et l'échantillonnage des textures.
    L'objectif de ce devoir est de développer les opérateurs d'acces et de ré-échantillonnage des textures.
    Le sujet détaillé sera accessible via la page \ref Textures.

    Avant d'avoir réalisé votre devoir, en utilisant la scene de test construite par la fonction make_textured_scene() (cf. fichier main.cpp), le code que vous avez développé en TP doit vous donner l'image suivante :
    \image html textures-initiale.png

    Après avoir écrit la première fonction demandée dans le devoir, vous devriez avoir l'image suivante.
   Vous remarquerez les effets du mauvais échantillonnage de la texture qui se traduit, pour les pixels proches de la camera par un sur-echantillonnage et donc, des lignes qui sont en escalier et pour les pixels lointains,
    par un sous-echantillonnage entrainant une perte d'information (les lignes noires disparaissent par endroit) et un effet de moiré.
    \image html textures-sansfiltrage.png

    Après avoir écrit votre deuxième fonction, les problèmes de sur-échantillonnage seront résolus par une reconstruction par interpolation linéaire de la texture
    (dont l'effet est surtout visible ici sur la gauche ou sur la droite de l'image)
    \image html textures-interpolation.png

    Après avoir écrit votre troisième fonction, les problèmes de sous-échantillonnage seront résolus (avec certains défauts liés à la méthode que l'on vous demande d'implanter) par une reconstruction par intégration
    de la texture sur un voisinage dépendant du pixel. (dont l'effet est surtout visible pour les objets lointains) Attention toutefois, le temps de calcul peut être long pour cette méthode directe
    (d'un facteur de 2 à 10 en fonction de la taile des textures, cf. le fichier main.cpp)
    \image html textures-integration.png

    Après avoir écrit votre quatrième fonction, les problèmes de sous-échantillonnage (dont l'effet est surtout visible pour les objets lointains) seront mieux résolus (meme s'il reste quelques défauts liés à la méthode que l'on vous demande d'implanter)
    par un préfiltrage des textures, tout en conservant un temps de calcul constant par pixel.
    \image html textures-mipmap.png

*/

/** @page Textures Textures et échantillonnage
  * En synthèse d'images, appliquer une texture sur un objet consiste à définir une fonction permettant de faire varier les paramètres d'apparence de cet objet sur sa surface.
  * Traditionellement, une texture est représentée par une image 2D et est utilisée pour faire varier la couleur sur la surface de l'objet.
  *
  *
  * Une image 2D est représenté par un tableau tableau 1D de pixels.
  * Chaque pixel est représenté par un nombre de valeur (composante) dépendant de son format
  *     niveau de gris : 1 composante
  *     RVB : 3 composante
  *     RVBA : 4 composante
  * le nombre de composantes utilisées pour représenté un pixel est appelé "depth".
  *    Le type des composantes est unsigned char permettant de représenter un entier non signé borné entre 0 et 255
  *
  * Une image 2D de taille  width*height et dons les pixels possèdent depth composante est donc représentée par un tableau
  * 1D de taille width*height*depth.
  * Au sein de ce tableau, les pixels sont organisés de façon linéaire par ligne de l'image. Les width*depth premières
  * valeurs correspondent aux pixels de la première ligne, les width*height suivante à la deuxième ligne et ainsi de suite.
  * Les composantes des pixels sont stockées les unes à la suite des autres selon une organisation tableau de structures. Ainsi,
  * les "depth" première valeurs de l'images correspondent aux composantes de 1er pixel, les depth suivantes à celle du
  * deuxième epixel et ainsi de suite.
  *
  * En dehors de ce format brut de stockage dans une image, les couleurs, dans notre mini logiciel de lancer de rayons,
  *  sont représentées par la classe Color fournissant les opérateurs arythmétiques élémentaires permettant de
  * faire la somme de deux couleurs, le produit de deux couleurs, le produtit d'une couleur par un scalaire (de façon commutative)
  * etc ... La classe couleur représente une couleur par un nombre à virgule flottante entre 0. et 1.
  *
  * L'objectif de ce devoir est de programmer les opérateurs getTexel(), interpolateTexture(), integrateTexture() et prefilterTexture  permettant d'associer une texture (une fonction discrétisée
  * dont les valeurs sont représentées par une image) à une géométrie et de regler les problèmes d'échantillonnage-rééchantillonnage
  *  résultants de la discrétisation de la scène par les rayons lancés à travers le plan image.
  *
  * Le sujet complet du devoir à réalisé est disponible sous moodle et à partir du lien <A HREF="../OIM-devoir3-2014.pdf">OIM-devoir3-2014.pdf</a>.
  *
  */


/**
 * @brief getTexel Get a single texel from a texture.
 * This function access the texture at coordinates (row, column) and fetch the value of the corresponding texel (pixel from a texture)
 * @param pixels    The image to access, organized as a linear array of texel arranged by row
 * @param width     Width of the image
 * @param height    Height of the image
 * @param depth     Depth of the image (number of component by texel)
 * @param column    Column coordinate of the requested texel
 * @param row       Row coordinate of the requested texel
 * @return          The value of the texel
 * @todo            Transfom the 2D coordinates in a 1D index and get the corresponding texel value
 */
Color getTexel(unsigned char *pixels, int width, int height, int depth, int column, int row);

/**
 * @brief interpolateTexture Get a texel linearly interpolated from its neighbors
 * @param pixels    The image to access, organized as a linear array of texel arranged by row
 * @param width     Width of the image
 * @param height    Height of the image
 * @param depth     Depth of the image (number of component by texel)
 * @param s         The column coordinate of the requested texel as a floating point
 * @param t         The row coordinate of the requested texel as a floating point
 * @return          The value of the interpolated texel
 * @todo            From the floating point coordinates, compute the integer part and the fractional part.
 *                  The integer part is the used to acces the 4 texels implied in the interpolation (One texel and its 3 up and right neighors), the fractional part is used to linearly interpolate from neighbors.
 */
Color interpolateTexture(unsigned char *pixels, int width, int height, int depth, float s, float t);


/**
 * @brief integrateTexture Get a texel by computing the mean of the color on a neighborood of size (deltas x deltat)
 * @param pixels    The image to access, organized as a linear array of texel arranged by row
 * @param width     Width of the image
 * @param height    Height of the image
 * @param depth     Depth of the image (number of component by texel)
 * @param s         The column coordinate of the requested texel as a floating point
 * @param t         The row coordinate of the requested texel as a floating point
 * @param deltas    The size, in the column dimension, of the neighborood
 * @param deltat    The size, in the row dimension, of the neighborood
 * @return
 * @todo
 */
Color integrateTexture(unsigned char *pixels, int width, int height, int depth, float s, float t, int deltas, int deltat);

/**
 * @brief prefilterTexture Compute an array of images with geometrically decreasing resolution from the original image.
 * @param imagearray The array of images to compute. element at index 0 in this array is the full resolution image and must not be modified
 * @param width     Width of the full resolution image
 * @param height    Height of the full resolution image
 * @param depth     Depth of the image (number of component by texel)
 * @param nblevels  Number of level to compute : nblevels = log2(min(width, height))
 * @return          if the array may be filled, return true, else return false
 */
bool prefilterTexture(unsigned char **imagearray, int width, int height, int depth, int nblevels);
