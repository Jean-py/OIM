#include "raytracing.h"

#include "scene.hpp"
#include "image.hpp"

#include "plane.hpp"
#include "sphere.hpp"
#include "cylinder.hpp"
#include "cone.hpp"
#include "box.hpp"
#include "csg.hpp"

#include "matte.hpp"
#include "plastic.hpp"
#include "metal.hpp"
#include "glass.hpp"
#include "glossy.hpp"

#include "texture.hpp"

//====================
//global variables

/// The scene
Scene g_scene;

/// The camera
Camera g_camera;

/// The resulting image
Image * g_p_img = NULL;


//====================
//scene description

/**
   to set the parameters of the camera
   @param pos_x_ position of the camera
   @param pos_y_
   @param pos_z_
   @param aim_x_ position of the point aimed
   @param aim_y_
   @param aim_z_
   @param up_x_ coordinates of the up vector
   @param up_y_
   @param up_z_
   @param x_res_ resolution of the image
   @param y_res_
*/
void set_camera (float pos_x_, float pos_y_, float pos_z_,
				 float aim_x_, float aim_y_, float aim_z_,
				 float up_x_, float up_y_, float up_z_,
				 float fov_, int x_res_, int y_res_) {
	g_camera = Camera (Vector3D (pos_x_, pos_y_, pos_z_),
					   Vector3D (aim_x_, aim_y_, aim_z_),
					   Vector3D (up_x_, up_y_, up_z_),
					   fov_, x_res_, y_res_);

	g_scene.set_camera (&g_camera);

	delete g_p_img;

	g_p_img = new Image (x_res_, y_res_);
}

/**
   to add a light to the scene
   @param pos_x_ position of the light
   @param pos_y_
   @param pos_z_
   @param r_ color of the light
   @param g_
   @param b_
*/
void add_light (float pos_x_, float pos_y_, float pos_z_,
				float r_, float g_, float b_) {
	g_scene.add_light (new Light (Vector3D (pos_x_, pos_y_, pos_z_),
								  Color (r_, g_, b_)));
}

/**
   to create a plane
   @param pos_x_ position of the plane
   @param pos_y_
   @param pos_z_
   @param normal_x_ normal of the plane
   @param normal_y_
   @param normal_z_
   @return a new geometry corresponding to the plane.
*/
Geometry * create_plane (float pos_x_, float pos_y_, float pos_z_,
						 float normal_x_, float normal_y_, float normal_z_) {
	return new Plane (Vector3D (pos_x_, pos_y_, pos_z_),
					  Vector3D (normal_x_, normal_y_, normal_z_));
}

/**
   to create a sphere
   @param pos_x_ position of the sphere
   @param pos_y_
   @param pos_z_
   @param radius_ its radius
   @return a new geometry corresponding to the sphere
*/
Geometry * create_sphere (float pos_x_, float pos_y_, float pos_z_,
						  float radius_) {
	return new Sphere (Vector3D (pos_x_, pos_y_, pos_z_), radius_);
}

/**
   to create a cylinder
   @param pos_x_ position of the cylinder (center of the cylinder)
   @param pos_y_
   @param pos_z_
   @param axis_x_ axis of the cylinder (direction)
   @param axis_y_
   @param axis_z_
   @param radius_ radius of the cylinder
   @param length_ length of the cylinder
   @return a new geometry corresponding to the cylinder
*/
Geometry * create_cylinder (float pos_x_, float pos_y_, float pos_z_,
                            float axis_x_, float axis_y_, float axis_z_,
                            float radius_, float length_) {
        return new Cylinder (Vector3D (pos_x_, pos_y_, pos_z_), Vector3D (axis_x_, axis_y_, axis_z_), radius_, length_);
}


/**
   to create a cone
   @param pos_x_ position of the cone (center of the base)
   @param pos_y_
   @param pos_z_
   @param vertex_x_ vertex of the cone
   @param vertex_y_
   @param vertex_z_
   @param radius_ radius of the cone at its base
   @return a new geometry corresponding to the cone
*/
Geometry * create_cone (float pos_x_, float pos_y_, float pos_z_,
                        float vertex_x_, float vertex_y_, float vertex_z_,
                        float radius_){
    return new Cone (Vector3D(pos_x_, pos_y_, pos_z_), Vector3D(vertex_x_, vertex_y_, vertex_z_), radius_);
}

/**
   to create a box
   @param pos_x_ position of the box (center of the box)
   @param pos_y_
   @param pos_z_
   @param x_base_x_ X vector of the base plane
   @param x_base_y_
   @param x_base_z_
   @param y_base_x_ Y vector of the base plane
   @param y_base_y_
   @param y_base_z_
   @param extent_x_ size on the X direction
   @param extent_y_ size on the Y direction
   @param extent_z_ size on the Z direction
   @return a new geometry corresponding to the box
*/
Geometry * create_box (float pos_x_, float pos_y_, float pos_z_,
                       float x_base_x_, float x_base_y_, float x_base_z_,
                       float y_base_x_, float y_base_y_, float y_base_z_,
                       float extent_x_, float extent_y_, float extent_z_
                       ){
    return new Box3D (Vector3D(pos_x_, pos_y_, pos_z_),
                    Vector3D(x_base_x_, x_base_y_, x_base_z_),
                    Vector3D(y_base_x_, y_base_y_, y_base_z_),
                    Vector3D(extent_x_, extent_y_, extent_z_));
}

/**
   to create a matte material
   @param kd_r_ the color of the material
   @param kd_g_
   @param kd_b_
   @return a new material corresponding to the matte material
*/
Material * create_matte_mat (float kd_r_, float kd_g_, float kd_b_) {
	return new Matte (Color (kd_r_, kd_g_, kd_b_));
}

/**
   to create a plastic material
   @param kd_r_ the color of the diffuse layer
   @param kd_g_
   @param kd_b_
   @param ks_r_ the color of the shiny layer
   @param ks_g_
   @param ks_b_
   @param shininess_ the higher, the more spiky the highlight
   @return a new material corresponding to the plastic material
*/
Material * create_plastic_mat (float kd_r_, float kd_g_, float kd_b_,
							   float ks_r_, float ks_g_, float ks_b_,
							   float shininess_) {
	return new Plastic (Color (kd_r_, kd_g_, kd_b_),
						Color (ks_r_, ks_g_, ks_b_),
						shininess_);
}

Material * create_glossy_mat (float kd_r_, float kd_g_, float kd_b_,
                               float ks_r_, float ks_g_, float ks_b_, float kr_r_, float kr_g_, float kr_b_,
                               float shininess_, float ior_){
    return new Glossy(Color (kd_r_, kd_g_, kd_b_),
                      Color (ks_r_, ks_g_, ks_b_), Color (kr_r_, kr_g_, kr_b_), ior_,
                      shininess_);
}

/**
   to create a metal-like material
   @param kd_r_ the color of the diffuse layer
   @param kd_g_
   @param kd_b_
   @param kr_r_ the color of the reflective layer
   @param kr_g_
   @param kr_b_
   @return a new material corresponding to the metal material
*/
Material * create_metal_mat (float kd_r_, float kd_g_, float kd_b_,
							 float kr_r_, float kr_g_, float kr_b_) {
	return new Metal (Color (kd_r_, kd_g_, kd_b_),
					  Color (kr_r_, kr_g_, kr_b_));
}

/**
   to create a glass-like material
   @param kr_r_ the color of the reflective layer
   @param kr_g_
   @param kr_b_
   @param kt_r_ the color of the transmission layer
   @param kt_g_
   @param kt_b_
   @param ior_ the index of refraction of the object
   @return a new material corresponding to the glass material
*/
Material * create_glass_mat (float ks_r_, float ks_g_, float ks_b_,
							 float kt_r_, float kt_g_, float kt_b_,
							 float ior_) {
	return new Glass (Color (ks_r_, ks_g_, ks_b_),
					  Color (kt_r_, kt_g_, kt_b_),
					  ior_);
}

/**
   to add a new object to the scene, from its geometry and its material
   @param geom_
   @param mat_
*/
void add_object (Geometry * geom_, Material * mat_) {
	g_scene.add_primitive (new Primitive (geom_, mat_));
}

/**
   to initialize the scene with the default one.
*/
void make_default_scene (int xsize, int ysize) {
	//camera
    set_camera (0.f, 2.f, -6.f,
				0.f, 0.f, 0.f,
				0.f, 1.f, 0.f,
                90.f, xsize, ysize);

	//objects

        Material * matte = create_matte_mat (1.f, 0.2f, 0.2f);
        Material * plastic = create_plastic_mat (0.6f, 0.5f, 0.5f,
                                                      0.6f, 0.5f, 0.5f,
                                                      100.f);

        Material * groundMaterial = create_plastic_mat (0.6f, 0.6f, 0.6f,
                                                      0.2f, 0.2f, 0.2f,
                                                      40.f);

        Material * metal = create_metal_mat (0.2f, 0.2f, 0.2f,
                                                 1.f, 1.f, 1.f);

        Material * glass = create_glass_mat (1.f, 1.f, 1.f,
                                             1.f, 1.f, 1.f,
                                             1.25f);
        Material * transp = create_glass_mat (1.f, 1.f, 1.f,
                                              1.f, 1.f, 1.f,
                                              1.f);


        Geometry * sphere1 = create_sphere (-1.f, 0.f, -1.f, 1.f);
        Geometry * sphere2 = create_sphere (-1.f, 0.f, 1.f, 1.f);
        Geometry * sphere3 = create_sphere (1.f, 0.f, -1.f, 1.f);
        Geometry * sphere4 = create_sphere (1.f, 0.f, 1.f, 1.f);

        Geometry * sphere_top = create_sphere (0.f, 1.414f, 0.f, 1.f);

        Geometry * plane = create_plane (0.f, -1.f, 0.f,
                                         0.f, 1.f, 0.f);
	
        Geometry * cylinder1 = create_cylinder(0.f, 2.f, -0.25f,
                                               1.f, -1.f, -1.f,
                                               .25f, 3.f );

        Geometry * cylinder2 = create_cylinder(0.f, 2.f, -0.25f,
                                               1.f, 1.f, 1.f,
                                               .25f, 3.f );

        add_object (sphere1, metal);
        add_object (sphere2, matte);
        add_object (sphere3, glass);
        add_object (sphere4, plastic);

        add_object (sphere_top, transp);

        add_object (plane, groundMaterial);

        add_object (cylinder1, plastic);
        add_object (cylinder2, plastic);



        //lights
        add_light (4.f, 2.f, 10.f,
                    0.5f, 0.5f, 0.5f);
 //                          0.1f, 0.1f, 0.9f);
        add_light (0.f, 5.f, -1.5f,
                           1.5f, 1.5f, 1.5f);
        add_light (-4.f, 5.f, -2.f,
                   0.5f, 0.5f, 0.5f);
//                           0.9f, 0.1f, 0.1f);
}

//====================
//access to scene elements

/**
   @return the number of lights in the scene
*/
int nb_lights () {
	return g_scene.p_lights().size();
}

/**
   @param i_
   @return the pointer to the ith light
*/
Light * get_light (int i_) {
	return g_scene.p_lights()[i_];
}

/**
   to get the resolution of the image
   @param p_x_res_  will be filled
   @param p_y_res_
*/
void get_image_resolution (int * p_x_res_, int * p_y_res_) {
	*p_x_res_ = g_camera.x_res();
	*p_y_res_ = g_camera.y_res();
}

//====================
//rays-related operations

/**
   compute the ray for the given pixel
   @param x_
   @param y_
   @return
*/
Ray camera_ray (int x_, int y_) {
	return g_camera.ray_for_pixel (x_, y_);
}

/**
   intersect a given ray with the scene.
   @param p_ray_ a pointer to the ray which has to be intersected, is modified.
   @param p_isect_ a pointer to the intersection structure, will be filled
   @return 0 if no intersection is found, 1 otherwise.
*/
int intersect_scene (Ray * p_ray_, Isect * p_isect_) {
	return g_scene.intersect (*p_ray_, p_isect_) ? 1 : 0;
}

/**
   test the visibility between an intersection and a light
   @param isect_
   @param light_
   @return 0 the light and the intersection are NOT mutually visible
   1 they are mutually visible
*/
int test_visibility (Isect isect_, Light * light_) {
	return g_scene.test_visibility (isect_.dg().pos(), light_) ? 1 : 0;
}

/**
   the depth of the ray in the ray tree
   @param ray_
   @return
*/
int ray_depth (Ray ray_) {
	return ray_.depth();
}

/**
   the importance of the ray
   @param ray_
   @return
*/
float ray_importance (Ray ray_) {
	return ray_.importance().avg();
}

//====================
//color-related

/**
   return a black color
*/
Color black() {
	return Color (0.f);
}

/**
   init a color
   @param r_
   @param g_
   @param b_
*/
Color init_color(float r_, float g_, float b_) {
	return Color (r_, g_, b_);
}

/**
   multiply colors component-wise
   @param c1_
   @param c2_
   @return
*/
Color multiply_color_color (Color c1_, Color c2_) {
	return c1_ * c2_;
}
  
/**
   add two colors
   @param c1_
   @param c2_
   @return
*/
Color add_color_color (Color c1_, Color c2_) {
	return c1_ + c2_;
}

/**
   @param color_
   @return 1 if the color is black, 0 otherwise
*/
int color_is_black (Color color_) {
	return color_.is_zero() ? 1 : 0;
}


//====================
//light-transport

/**
   @param isect_
   @return 0 if no reflection has to be computed,
   1 otherwise
*/
int isect_has_reflection (Isect isect_) {
	return isect_.bsdf().has_reflection();
}

/**
   @param isect_
   @return 0 if no refraction has to be computed,
   1 otherwise
*/
int isect_has_refraction (Isect isect_) {
	return isect_.bsdf().has_refraction();
}

/**
   compute the reflected ray from the intersection data and the ray to the intersection
   @param ray_
   @param isect_
   @param p_reflected_ray_ will be filled
   @return proportion of reflected color
*/
Color reflect (Ray ray_, Isect isect_, Ray * p_ray_) {
	Vector3D refl_dir;
	Color fs = isect_.bsdf().reflect_dir (-ray_.dir(), &refl_dir);

	*p_ray_ = Ray (ray_, fs, isect_.dg().pos(), refl_dir);
    // Update ray differentials on p_ray_

    Vector3D dPpdx = ray_.dPdx();
    Vector3D dPpdy = ray_.dPdy();


    float dDdotNdx = ray_.dDdx().dot(isect_.dg().normal()) + ray_.dir().dot(isect_.dg().dNdx());
    float dDdotNdy = ray_.dDdy().dot(isect_.dg().normal()) + ray_.dir().dot(isect_.dg().dNdy());

    Vector3D dDpdx(ray_.dDdx() - ((isect_.dg().dNdx()*ray_.dir().dot(isect_.dg().normal()) +  isect_.dg().normal() * dDdotNdx) *  2.f) );
    Vector3D dDpdy(ray_.dDdy() - ((isect_.dg().dNdy()*ray_.dir().dot(isect_.dg().normal()) +  isect_.dg().normal() * dDdotNdy) *  2.f) );

    p_ray_->setdPdx(dPpdx);
    p_ray_->setdPdy(dPpdy);
    p_ray_->setdDdx(dDpdx);
    p_ray_->setdDdy(dDpdy);

	return fs * fabsf (isect_.dg().normal().dot (refl_dir)); //cos(theta) here for easier use
}
  
/**
   compute the refracted ray from the intersection data and the ray to the intersection
   @param ray_
   @param isect_
   @param p_refracted_ray_ will be filled
   @return proportion of refracted color
*/
Color refract (Ray ray_, Isect isect_, Ray * p_ray_) {

    Vector3D refr_dir;
	Color fs = isect_.bsdf().refract_dir (-ray_.dir(), &refr_dir);

	*p_ray_ = Ray (ray_, fs, isect_.dg().pos(), refr_dir);
    //Update ray differentials on p_ray_

    bool entering = (ray_.dir().dot(isect_.dg().normal()) > 0.f);
    float ior=isect_.bsdf().getIor(entering);
    float mu = ior * ray_.dir().dot(isect_.dg().normal()) - refr_dir.dot(isect_.dg().normal());

    Vector3D dPpdx = ray_.dPdx();
    Vector3D dPpdy = ray_.dPdy();
    float dDdotNdx = ray_.dDdx().dot(isect_.dg().normal()) + ray_.dir().dot(isect_.dg().dNdx());
    float dDdotNdy = ray_.dDdy().dot(isect_.dg().normal()) + ray_.dir().dot(isect_.dg().dNdy());

    float dmudx = (ior - (ior*ior*ray_.dir().dot(isect_.dg().normal())/ refr_dir.dot(isect_.dg().normal())))*dDdotNdx;
    float dmudy = (ior - (ior*ior*ray_.dir().dot(isect_.dg().normal())/ refr_dir.dot(isect_.dg().normal())))*dDdotNdy;


    Vector3D dDpdx( ray_.dDdx() * ior - (isect_.dg().dNdx() * mu + isect_.dg().normal() * dmudx) );
    Vector3D dDpdy( ray_.dDdy() * ior - (isect_.dg().dNdy() * mu + isect_.dg().normal() * dmudy) );

    p_ray_->setdPdx(dPpdx);
    p_ray_->setdPdy(dPpdy);
    p_ray_->setdDdx(dDpdx);
    p_ray_->setdDdy(dDpdy);

	return fs * fabsf (isect_.dg().normal().dot (refr_dir)); //cos(theta) here for easier use
}

/**
   compute the amount of color scattered at the intersection
   to the ray from the given light, assuming there is no occluder
   @param ray_
   @param isect_
   @param light_
*/
Color direct_lighting (Ray ray_, Isect isect_, Light * light_) {
	Vector3D dir_to_light = (light_->pos() - isect_.dg().pos()).normalize();
	return isect_.bsdf().evaluate (-ray_.dir(), dir_to_light) * light_->le() *
        fabsf (isect_.dg().normal().dot (dir_to_light));
}


//====================
//Image management

/**
   to set the color of the given pixel
   @param x_
   @param y_
   @param color_
*/
void set_pixel_color (int x_, int y_, Color color_) {
	g_p_img->pixel (x_, y_) = color_;
}

/**
   to save the image to a file
   @param file_name_
*/
void save_image (const char * file_name_) {
	g_p_img->write_to_exr_file (std::string (file_name_));
}

void save_png (const char * file_name_) {
    g_p_img->write_to_png_file(file_name_);
}

//------------------
// CSG

Geometry *create_csg_intersection(Geometry *a, Geometry *b){
    return new CSGIntersection(a,b);
}

Geometry *create_csg_union(Geometry *a, Geometry *b){
    return new CSGUnion(a,b);
}

Geometry *create_csg_difference(Geometry *a, Geometry *b){
    return new CSGDifference(a,b);
}

//------------------
// Texture
Texture *create_texture(const char *filename){
	Texture *tex;
	tex = new Texture(filename);
    return tex;
}

