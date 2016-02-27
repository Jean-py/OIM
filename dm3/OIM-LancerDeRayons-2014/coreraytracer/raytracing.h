#pragma once

#include "geometry.hpp"
#include "material.hpp"
class Light;
class Texture;

#include "ray.hpp"
#include "isect.hpp"
#include "color.hpp"

//extern "C" {
	
/** @defgroup RaytraceAPI Simple Raytracing API
 *  Application Programming Interface for the Very Simple Ray Tracer
 *  @{
 */
	
	/** @defgroup SceneAPI Scene definition API
	 *  Application Programming Interface for the building simple scenes
	 *  @{
	 */
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
	 @param fov_ the field of view, in degrees
	 @param x_res_ resolution of the image
	 @param y_res_
  */
  void set_camera (float pos_x_, float pos_y_, float pos_z_,
				   float aim_x_, float aim_y_, float aim_z_,
				   float up_x_, float up_y_, float up_z_,
				   float fov_, int x_res_, int y_res_);

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
				  float r_, float g_, float b_);

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
						   float normal_x_, float normal_y_, float normal_z_);

  /**
	 to create a sphere
	 @param pos_x_ position of the sphere
	 @param pos_y_
	 @param pos_z_
	 @param radius_ its radius
	 @return a new geometry corresponding to the sphere
  */
  Geometry * create_sphere (float pos_x_, float pos_y_, float pos_z_,
							float radius_);

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
                              float radius_, float length_);

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
                          float radius_);

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
                         );

  /**
	 to create a matte material
	 @param kd_r_ the color of the material
	 @param kd_g_
	 @param kd_b_
	 @return a new material corresponding to the matte material
  */
  Material * create_matte_mat (float kd_r_, float kd_g_, float kd_b_);

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
								 float shininess_);

  /**
     to create a glossy material
     @param kd_r_ the color of the diffuse layer
     @param kd_g_
     @param kd_b_
     @param ks_r_ the color of the shiny layer
     @param ks_g_
     @param ks_b_
     @param shininess_ the higher, the more spiky the highlight
     @param ior_ the index of refraction of the object
     @return a new material corresponding to the plastic material
  */
  Material * create_glossy_mat (float kd_r_, float kd_g_, float kd_b_,
                                 float ks_r_, float ks_g_, float ks_b_,
                                 float kr_r_, float kr_g_, float kr_b_,
                                 float shininess_, float ior_);

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
							   float kr_r_, float kr_g_, float kr_b_);

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
							   float ior_);

  /**
	 to add a new object to the scene, from its geometry and its material
	 @param geom_
	 @param mat_
  */
  void add_object (Geometry * geom_, Material * mat_);

  /**
	 to initialize the scene with the default one.
         @param xsize width of the picture
         @param ysize heigth of the picture
  */
  void make_default_scene (int xsize, int ysize);

  //====================
  //access to scene elements

  /**
	 @return the number of lights in the scene
  */
  int nb_lights ();

  /**
	 @param i_
	 @return the pointer to the ith light
  */
  Light * get_light (int i_);


	/** @} */ // end of SceneAPI group
	
	/** @defgroup RayAPI Ray management API
	 *  Application Programming Interface for the rays management
	 *  @{
	 */
	
  //====================
  //rays-related operations

  /**
	 compute the ray for the given pixel
	 @param x_
	 @param y_
	 @return
  */
  Ray camera_ray (int x_, int y_);

  /**
	 intersect a given ray with the scene.
	 @param p_ray_ a pointer to the ray which has to be intersected, is modified.
	 @param p_isect_ a pointer to the intersection structure, will be filled
	 @return 0 if no intersection is found, 1 otherwise.
  */
  int intersect_scene (Ray * p_ray_, Isect * p_isect_);

  /**
	 test the visibility between an intersection and a light
	 @param isect_
	 @param light_
	 @return 0 the light and the intersection are NOT mutually visible
	 1 they are mutually visible
  */
  int test_visibility (Isect isect_, Light * light_);

  /**
	 the depth of the ray in the ray tree
	 @param ray_
	 @return
  */
  int ray_depth (Ray ray_);

  /**
	 the importance of the ray
	 @param ray_
	 @return
  */
  float ray_importance (Ray ray_);
	/** @} */ // end of RayAPI group

	/** @defgroup ColorAPI Color management API
	 *  Application Programming Interface for color management
	 *  @{
	 */
	
  //====================
  //color-related

  /**
	 return a black color
  */
  Color black();

  /**
	 init a color
	 @param r_
	 @param g_
	 @param b_
  */
  Color init_color(float r_, float g_, float b_);

  /**
	 multiply colors component-wise
	 @param c1_
	 @param c2_
	 @return
  */
  Color multiply_color_color (Color c1_, Color c2_);
  
  /**
	 add two colors
	 @param c1_
	 @param c2_
	 @return
  */
  Color add_color_color (Color c1_, Color c2_);

  /**
	 @param color_
	 @return 1 if the color is black, 0 otherwise
  */
  int color_is_black (Color color_);
  
	/** @} */ // end of ColorAPI group

	/** @defgroup LightAPI Lighting computation API
	 *  Application Programming Interface for lighting computation
	 *  @{
	 */
	

  //====================
  //light-transport

  /**
	 @param isect_
	 @return 0 if no reflection has to be computed,
	 1 otherwise
  */
  int isect_has_reflection (Isect isect_);

  /**
	 @param isect_
	 @return 0 if no refraction has to be computed,
	 1 otherwise
  */
  int isect_has_refraction (Isect isect_);

  /**
	 compute the reflected ray from the intersection data and the ray to the intersection
	 @param ray_
	 @param isect_
	 @param p_reflected_ray_ will be filled
	 @return proportion of reflected color
  */
  Color reflect (Ray ray_, Isect isect_, Ray * p_ray_);
  
  /**
	 compute the refracted ray from the intersection data and the ray to the intersection
	 @param ray_
	 @param isect_
	 @param p_refracted_ray_ will be filled
	 @return proportion of refracted color. it can be black in case of total internal reflection
  */
  Color refract (Ray ray_, Isect isect_, Ray * p_ray_);  

  /**
	 compute the amount of color scattered at the intersection
	 to the ray from the given light, assuming there is no occluder
	 @param ray_
	 @param isect_
	 @param light_
  */
  Color direct_lighting (Ray ray_, Isect isect_, Light * light_);

	/** @} */ // end of LightingAPI group


	/** @defgroup ImageAPI Image management API
	 *  Application Programming Interface for image management
	 *  @{
	 */
	
	//==============
	//======
  //Image management
	/**
	 to get the resolution of the image
	 @param p_x_res_  will be filled
	 @param p_y_res_
	 */
	void get_image_resolution (int * p_x_res_, int * p_y_res_);
	
  /**
	 to set the color of the given pixel
	 @param x_
	 @param y_
	 @param color_
  */
  void set_pixel_color (int x_, int y_, Color color_);

  /**
	 to save the image to a file
	 @param file_name_
  */
  void save_image (const char *file_name_);

  void save_png (const char * file_name_);

	/** @} */ // end of ImageAPI group

/** @} */ // end of RaytraceAPI group


  // CSG

  Geometry *create_csg_intersection(Geometry *a, Geometry *b);
  Geometry *create_csg_union(Geometry *a, Geometry *b);
  Geometry *create_csg_difference(Geometry *a, Geometry *b);


  Texture *create_texture(const char *filename);

//}

