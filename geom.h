#ifndef _geom_h_
#define _geom_h_

#include "precision.h"

#define GEOM_NO_ERROR 0
#define GEOM_ERR_NOT_IMPLEMENTED 1
#define GEOM_ERR_ZERO_VECTOR 2
typedef float Real;


struct Transform {
	Real rot[9];
	Real trans[3];
};

struct Bbox {
	Real min[3];
	Real max[3];
};

// util
Real geom_deg2rad(Real angle_deg);
Real geom_rad2deg(Real angle_rad);
void geom_calc_normal(const Real* v0, const Real* v1, const Real* v2, Real* vnret);
Real geom_distance_point_line(const Real* point, const Real* line_beg, const Real* line_end);
int geom_get_tri_plane(const Real* tri, Real* plane_ret);

int geom_line_triplane_intersect(const Real* line, const Real* tri, Real* intersect_pt_ret);

/*
	descrip:
		Calculates intersection point of param line and param triangle.

	params:
		line: Real[2*3] - array of coords for 2 3D points
		tri:  Real[3*3] - array of coords for 3 3D points
		intersect_pt_ret: Real[3] - array of coords for 1 3D point

	returns:
		if line segment intersects tri: 1
		else: 0
*/
int geom_line_tri_intersect(const Real* line, const Real* tri, Real* intersect_pt_ret);

// Transform
void geom_transform_tostring(const struct Transform* tfm, char* s_ret);

// vector3
Real geom_vector3_magnitude(const Real* v);
int geom_vector3_set(Real* vret, Real x, Real y, Real z);
int	geom_vector3_scalar_mul(const Real* v, Real s, Real* vret);
int geom_vector3_copy(const Real* v, Real* vret);
int geom_vector3_zero(Real* vret);
int geom_vector3_add(const Real* v0, const Real* v1, Real* vret);
int geom_vector3_sub(const Real* v0, const Real* v1, Real* vret);
Real geom_vector3_distance(const Real* v0, const Real* v1);
Real geom_vector3_dot(const Real* v0, const Real* v1);
int geom_vector3_cross(const Real* v0, const Real* v1, Real* vret);
int geom_vector3_normalize(Real* vret);
void geom_vector3_tostring(const Real* v, char* s_ret);
int geom_vector3_equal(const Real* v0, const Real* v1);
int geom_vector3_min(const Real* v0, const Real* v1, Real* vmin_ret);
int geom_vector3_max(const Real* v0, const Real* v1, Real* vmax_ret);

// matrix3
int geom_matrix3_transposed(const Real* m, Real* mret);
int geom_matrix3_copy(const Real* m, Real* mret);
int	geom_matrix3_mul(const Real* m0, const Real* m1, Real* mr);
int	geom_vector3_matrix3_mul(const Real* v, const Real* m, Real* vret);
int geom_matrix3_new_ident(Real* mat_ret);
int geom_matrix3_new_rotx(Real angle_deg, Real* mat_ret);
int geom_matrix3_new_roty(Real angle_deg, Real* mat_ret);
int geom_matrix3_new_rotz(Real angle_deg, Real* mat_ret);
int geom_matrix3_new_rot(const Real* vrot, Real angle_deg, Real* mat_rot_ret);
void geom_matrix3_tostring(const Real* m, char* s_ret);

// matrix4
int geom_matrix4_new_ident(Real* mat_ret);
int geom_matrix3_matrix4_copy(const Real* mat3, Real* mat4_ret);
int geom_matrix4_copy(const Real* mat4, Real* mat4_ret);
int geom_vector3_matrix4_copy(const Real* vec3, Real* mat4_ret);
void geom_matrix4_tostring(const Real* m, char* s_ret);
int geom_vector3_matrix4_mul(const Real* vec3, const Real* mat4, Real* vec3_ret);
int geom_vector4_matrix4_mul(const Real* v, const Real* m, Real* vec4_ret);

#endif
