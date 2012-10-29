#include <math.h>
#include <string.h>
#include <stdio.h>

#include "geom.h"

#define MIN(a, b) ((a)<(b) ? (a) : (b))
#define MAX(a, b) ((a)>(b) ? (a) : (b))

Real geom_deg2rad(Real angle_deg) {
	return M_PI * angle_deg / 180;
}

Real geom_rad2deg(Real angle_rad) {
	return 180 * angle_rad / M_PI;
}

void geom_transform_tostring(const struct Transform* tfm, char* s_ret) {
	sprintf(s_ret, 
	"trans: (% f, % f, % f)\n  rot: (% f, % f, % f\n        % f, % f, % f\n        % f, % f, % f)", 
		tfm->trans[0], tfm->trans[1], tfm->trans[2], 
		tfm->rot[0], tfm->rot[1], tfm->rot[2], 
		tfm->rot[3], tfm->rot[4], tfm->rot[5], 
		tfm->rot[6], tfm->rot[7], tfm->rot[8]);
}

void geom_calc_normal(const Real* v0, const Real* v1, const Real* v2, Real* vnret) {
	Real v01[3];
	geom_vector3_sub(v1, v0, v01);
	Real v02[3];
	geom_vector3_sub(v2, v0, v02);
	geom_vector3_cross(v01, v02, vnret);
	geom_vector3_normalize(vnret);
}

/* Returns [a, b, c, d] where:
			ax + by + cz + d = 0 

	plane normal = [a, b, c]
	d = -n dot P, where P is a point in the plane

*/
int geom_get_tri_plane(const Real* tri, Real* plane_ret) {
	// calc normal
	Real vn[3];
	geom_calc_normal(tri+0*3, tri+1*3, tri+2*3, vn);
	Real d = -geom_vector3_dot(vn, tri+0*3);
	geom_vector3_copy(vn, plane_ret);
	plane_ret[3] = d;
	return GEOM_NO_ERROR; 
}

/* 
	from gtcg, page 483:

	t = -(n dot P0 + d) / (n dot v) where

		P0 = first point in line segment
		P1 = second point in line segment
		v = P1 - P0
		n = plane normal
		d = d in eqn of plane 'ax + by + cy + d'

	Q = P0 + tv

	Returns:
		1 if line segment intersects plane, else 0


*/
int geom_line_triplane_intersect(const Real* line, const Real* tri, Real* intersect_pt_ret) {
	Real plane[4];
	geom_get_tri_plane(tri, plane);

	const Real* P0 = line+0*3;
	const Real* P1 = line+1*3;
	Real v[3];
	geom_vector3_sub(P1, P0, v);
	Real n[3];
	geom_vector3_copy(plane, n);
	Real d = plane[3];
	Real denom = geom_vector3_dot(n, v);
	if(denom == 0)
		return 0;

	Real t = - (geom_vector3_dot(n, P0) + d) / denom;
	if(t < 0 || t > 1)
		return 0;
	Real tv[3];			
	geom_vector3_scalar_mul(v, t, tv);
	Real Q[3];
	geom_vector3_add(P0, tv, Q);
	
	geom_vector3_copy(Q, intersect_pt_ret);
	return 1;
}

/* from ctcg page 486:

| t |                   1                 | ((P - V0) cross (V1 - V0)) dot (V2 - V0)
| u | = _________________________________ | (d cross (V2 - V0)) dot (P - V0)
| v |   (d cross (V2 - V0)) dot (V1 - V0) | ((P - V0) cross (V1 - V0)) dot d

from rtr, page 749:

| t |                   1                 | r dot e2 |
| u | = _________________________________ | q dot s  |
| v | =              q dot e1             | r dot d  |

where
	e1 = T1 - T0
	e2 = T2 - T0
	d = P1 - P0
	s = P0 - T0
  q = d cross e2
	r = s cross e1

Returns:
	if line segment intersects tri: 1
	else: 0
*/
int geom_line_tri_intersect(const Real* line, const Real* tri, Real* intersect_pt_ret) {
int report_no_intersect = 0;
	Real P0[3];
	geom_vector3_copy(line + 3*0, P0);
	Real P1[3];
	geom_vector3_copy(line + 3*1, P1);
	Real d[3];
	geom_vector3_sub(P1, P0, d);

	Real T0[3];
	geom_vector3_copy(tri + 3*0, T0);
	Real T1[3];
	geom_vector3_copy(tri + 3*1, T1);
	Real T2[3];
	geom_vector3_copy(tri + 3*2, T2);

	Real e1[3];
	geom_vector3_sub(T1, T0, e1);
	Real e2[3];
	geom_vector3_sub(T2, T0, e2);

	Real q[3];
	geom_vector3_cross(d, e2, q);

	Real s[3];
	geom_vector3_sub(P0, T0, s);

	Real a = geom_vector3_dot(q, e1);
	Real epsilon = 1e-7;
	if(fabs(a) < epsilon) {
		if(report_no_intersect) 
			printf("  geom_line_tri_intersect(): no intersect, a < epsilon: %f", a);
		return 0;
	}
	a = 1.0 / a;

	Real r[3];
	geom_vector3_cross(s, e1, r);

	Real t = a * geom_vector3_dot(r, e2);
	if(t < 0 || t > 1) {
		if(report_no_intersect) 
			printf("  geom_line_tri_intersect(): no intersect, t = %f\n", t);
		return 0;
	}

	Real u = a * geom_vector3_dot(q, s);
	if(u < 0) {
		if(report_no_intersect) 
			printf("  geom_line_tri_intersect(): no intersect, u = %f\n", u);
		return 0;
	}

	Real v = a * geom_vector3_dot(r, d);
	if(v < 0 || (u + v) > 1) {
		if(report_no_intersect) 
			printf("  geom_line_tri_intersect(): no intersect, v = %f\n", v);
		return 0;
	}

	Real td[3];
	geom_vector3_scalar_mul(d, t, td);
	Real Q[3];
	geom_vector3_add(P0, td, Q);
	geom_vector3_copy(Q, intersect_pt_ret);
//printf("  intersect! t: %f, u: %f, v: %f, ipt: (%f, %f, %f)\n", t, u, v, Q[0], Q[1], Q[2]);
	return 1;
}

// --- vector3
Real geom_vector3_magnitude(const Real* v) {
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

int geom_vector3_set(Real* vret, Real x, Real y, Real z) {
	vret[0] = x;
	vret[1] = y;
	vret[2] = z;
	return GEOM_NO_ERROR; 
}

int	geom_vector3_scalar_mul(const Real* v, Real s, Real* vret) {
	vret[0] = v[0] * s; 
	vret[1] = v[1] * s; 
	vret[2] = v[2] * s;
	return GEOM_NO_ERROR;
}

int geom_vector3_copy(const Real* v, Real* vret) {
	return geom_vector3_set(vret, v[0], v[1], v[2]);
}

int geom_vector3_zero(Real* vret) {
	return geom_vector3_set(vret, 0, 0, 0);
}

int geom_vector3_add(const Real* v0, const Real* v1, Real* vret) {
	int i;
	for(i = 0; i < 3; ++i)
		vret[i] = v0[i] + v1[i];
	return GEOM_NO_ERROR; 
}

int geom_vector3_sub(const Real* v0, const Real* v1, Real* vret) {
	int i;
	for(i = 0; i < 3; ++i)
		vret[i] = v0[i] - v1[i];
	return GEOM_NO_ERROR; 
}

Real geom_vector3_distance(const Real* v0, const Real* v1) {
	Real v[3];
	geom_vector3_sub(v0, v1, v);
	return geom_vector3_magnitude(v);
}

Real geom_vector3_dot(const Real* v0, const Real* v1) {
	return v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2];
}

int geom_vector3_cross(const Real* v0, const Real* v1, Real* vret) {
	Real vr[3];
	vr[0] = v0[1]*v1[2] - v0[2]*v1[1];
	vr[1] = v0[2]*v1[0] - v0[0]*v1[2];
	vr[2] = v0[0]*v1[1] - v0[1]*v1[0];
	geom_vector3_copy(vr, vret);
	return GEOM_NO_ERROR; 
}

int geom_vector3_normalize(Real* vret) {
	Real m = geom_vector3_magnitude(vret);
	if(m == 0)
		geom_vector3_zero(vret);
	else {
		Real vn[3];
		int i;
		for(i = 0; i < 3; ++i)
			vn[i] = vret[i] / m;
		geom_vector3_copy(vn, vret);
	}
	return GEOM_NO_ERROR; 
}
void geom_vector3_tostring(const Real* v, char* s_ret) {
	sprintf(s_ret, 
	"%f %f %f", 
	v[0], v[1], v[2]);
}

int geom_vector3_equal(const Real* v0, const Real* v1) {
	return v0[0]==v1[0] && v0[1]==v1[1] && v0[2]==v1[2];
}

int geom_vector3_min(const Real* v0, const Real* v1, Real* vmin_ret) {
	int i;
	for(i = 0; i < 3; ++i)
		vmin_ret[i] = MIN(v0[i], v1[i]);
	return GEOM_NO_ERROR; 
}

int geom_vector3_max(const Real* v0, const Real* v1, Real* vmax_ret) {
	int i;
	for(i = 0; i < 3; ++i)
		vmax_ret[i] = MAX(v0[i], v1[i]);
	return GEOM_NO_ERROR; 
}

// --- matrix3
int geom_matrix3_copy(const Real* m, Real* mret) {
	int i;
	for(i = 0; i < 9; ++i)
		mret[i] = m[i];
	return GEOM_NO_ERROR; 
}

int geom_matrix3_transposed(const Real* m, Real* mret) {
	Real mr[9];
	mr[ 0] = m[ 0]; mr[ 1] = m[ 3]; mr[ 2] = m[ 6];
	mr[ 3] = m[ 1]; mr[ 4] = m[ 4]; mr[ 5] = m[ 7];
	mr[ 6] = m[ 2]; mr[ 7] = m[ 5]; mr[ 8] = m[ 8];
	geom_matrix3_copy(mr, mret);
	return GEOM_NO_ERROR;
}

int	geom_matrix3_mul(const Real* m0, const Real* m1, Real* mat_ret) {
	Real mr[9];
	mr[ 0] = m0[ 0]*m1[ 0] + m0[ 1]*m1[ 3] + m0[ 2]*m1[ 6];
	mr[ 1] = m0[ 0]*m1[ 1] + m0[ 1]*m1[ 4] + m0[ 2]*m1[ 7];
	mr[ 2] = m0[ 0]*m1[ 2] + m0[ 1]*m1[ 5] + m0[ 2]*m1[ 8];

	mr[ 3] = m0[ 3]*m1[ 0] + m0[ 4]*m1[ 3] + m0[ 5]*m1[ 6];
	mr[ 4] = m0[ 3]*m1[ 1] + m0[ 4]*m1[ 4] + m0[ 5]*m1[ 7];
	mr[ 5] = m0[ 3]*m1[ 2] + m0[ 4]*m1[ 5] + m0[ 5]*m1[ 8];

	mr[ 6] = m0[ 6]*m1[ 0] + m0[ 7]*m1[ 3] + m0[ 8]*m1[ 6];
	mr[ 7] = m0[ 6]*m1[ 1] + m0[ 7]*m1[ 4] + m0[ 8]*m1[ 7];
	mr[ 8] = m0[ 6]*m1[ 2] + m0[ 7]*m1[ 5] + m0[ 8]*m1[ 8];

	geom_matrix3_copy(mr, mat_ret);
	return GEOM_NO_ERROR;
}

int	geom_vector3_matrix3_mul(const Real* v, const Real* m, Real* vret) {
	Real vr[3];
	vr[0] = v[0]*m[ 0] + v[1]*m[ 3] + v[2]*m[ 6];
	vr[1] = v[0]*m[ 1] + v[1]*m[ 4] + v[2]*m[ 7];
	vr[2] = v[0]*m[ 2] + v[1]*m[ 5] + v[2]*m[ 8];
	geom_vector3_copy(vr, vret);
	return GEOM_NO_ERROR;
}

// Returns an identity matrix
int geom_matrix3_new_ident(Real* mat_ret) {
	mat_ret[ 0] = 1; mat_ret[ 1] = 0; mat_ret[ 2] = 0;
	mat_ret[ 3] = 0; mat_ret[ 4] = 1; mat_ret[ 5] = 0;
	mat_ret[ 6] = 0; mat_ret[ 7] = 0; mat_ret[ 8] = 1;
	return GEOM_NO_ERROR;
}

// Returns a matrix with rotation about x axis by param angle angle_deg
int geom_matrix3_new_rotx(Real angle_deg, Real* mat_ret) {
	Real theta = geom_deg2rad(angle_deg);
	mat_ret[ 0] = 1; mat_ret[ 1] = 0; mat_ret[ 2] = 0;
	mat_ret[ 3] = 0; mat_ret[ 4] = cos(theta); mat_ret[ 5] = sin(theta);
	mat_ret[ 6] = 0; mat_ret[ 7] = -sin(theta); mat_ret[ 8] = cos(theta);
	return GEOM_NO_ERROR;
}

// Returns a matrix with rotation about y axis by param angle angle_deg
int geom_matrix3_new_roty(Real angle_deg, Real* mat_ret) {
	Real theta = geom_deg2rad(angle_deg);
	mat_ret[ 0] = cos(theta); mat_ret[ 1] = 0; mat_ret[ 2] = -sin(theta);
	mat_ret[ 3] = 0; mat_ret[ 4] = 1; mat_ret[ 5] = 0;
	mat_ret[ 6] = sin(theta); mat_ret[ 7] = 0; mat_ret[ 8] = cos(theta);
	return GEOM_NO_ERROR;
}

// Returns a matrix with rotation about z axis by param angle angle_deg
int geom_matrix3_new_rotz(Real angle_deg, Real* mat_ret) {
	Real theta = geom_deg2rad(angle_deg);
	mat_ret[ 0] = cos(theta); mat_ret[ 1] = sin(theta); mat_ret[ 2] = 0;
	mat_ret[ 3] = -sin(theta); mat_ret[ 4] = cos(theta); mat_ret[ 5] = 0;
	mat_ret[ 6] = 0; mat_ret[ 7] = 0; mat_ret[ 8] = 1;
	return GEOM_NO_ERROR;
}

// Returns a matrix with rotation about param vector vrot by param angle angle_deg
int geom_matrix3_new_rot(const Real* vrot, Real angle_deg, Real* mat_rot_ret) {
	/* implementation:
	 create Mt * Rx * M where: 
			M is identity matrix transformed to (vrot, vrot_perp, vrot x vrot_perp)
			Rx is rotation matrix about cartesian x axis
	*/
	Real epsilon = 0.0000001;
	if( geom_vector3_magnitude(vrot) < epsilon )
		return GEOM_ERR_ZERO_VECTOR;
	Real vr[3];
	geom_vector3_copy(vrot, vr);	
	geom_vector3_normalize(vr);
	Real vc[3] = {1, 0, 0};
	Real vperp[3];
	geom_vector3_cross(vr, vc, vperp);
	if( geom_vector3_magnitude(vperp) < epsilon ) {
		geom_vector3_set(vc, 0, 1, 0);
		geom_vector3_cross(vr, vc, vperp);
	}
	geom_vector3_normalize(vperp);
	Real vperp2[3];
	geom_vector3_cross(vr, vperp, vperp2);
	geom_vector3_normalize(vperp2);
	Real M[9] = {
		vr[0], vr[1], vr[2],
		vperp[0], vperp[1], vperp[2],
		vperp2[0], vperp2[1], vperp2[2],
	};
	Real Rx[9];
	geom_matrix3_new_rotx(angle_deg, Rx);
	Real Mi[9];
	geom_matrix3_mul(Rx, M, Mi);
	Real Mt[9];
	geom_matrix3_transposed(M, Mt);
	geom_matrix3_mul(Mt, Mi, mat_rot_ret);
	return GEOM_NO_ERROR;
}

void geom_matrix3_tostring(const Real* m, char* s_ret) {
	int i;
	s_ret[0] = '\0';
	for(i = 0; i < 9; ++i) {
		char num[128];
		if(i && i % 3 == 0)
			strcat(s_ret, "\n");
		sprintf(num, "%f ", m[i]);
		strcat(s_ret, num);
	}
};

int geom_matrix4_new_ident(Real* m) {
	m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;	
	m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;	
	m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;	
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;	
	return GEOM_NO_ERROR;
}

int geom_matrix3_matrix4_copy(const Real* m3, Real* m) {
	m[ 0] = m3[ 0]; m[ 1] = m3[ 1]; m[ 2] = m3[ 2];
	m[ 4] = m3[ 3]; m[ 5] = m3[ 4]; m[ 6] = m3[ 5];
	m[ 8] = m3[ 6]; m[ 9] = m3[ 7]; m[10] = m3[ 8];
	return GEOM_NO_ERROR;
}

int geom_vector3_matrix4_copy(const Real* v3, Real* m) {
	m[12] = v3[0]; m[13] = v3[1]; m[14] = v3[2];
	return GEOM_NO_ERROR;
}

int geom_matrix4_copy(const Real* m4, Real* m) {
	memcpy(m, m4, sizeof(Real) * 16);
	return GEOM_NO_ERROR;
}

Real geom_distance_point_line(const Real* point, const Real* line_beg, const Real* line_end) {
	Real line_mag = geom_vector3_distance(line_beg, line_end);
	Real u = 
		((point[0] - line_beg[0]) * (line_end[0] - line_beg[0]) +
		(point[1] - line_beg[1]) * (line_end[1] - line_beg[1]) +
		(point[2] - line_beg[2]) * (line_end[2] - line_beg[2])) / (line_mag * line_mag);

	if(u < 0.0f || u > 1.0f)
			return -1;   // closest point does not fall within the line segment
 
	Real intersect[3];
	intersect[0] = line_beg[0] + u * (line_end[0] - line_beg[0]);
	intersect[1] = line_beg[1] + u * (line_end[1] - line_beg[1]);
	intersect[2] = line_beg[2] + u * (line_end[2] - line_beg[2]);

	Real d = geom_vector3_distance(point, intersect);
	return d;
}

void geom_matrix4_tostring(const Real* m, char* s_ret) {
	int i;
	s_ret[0] = '\0';
	for(i = 0; i < 16; ++i) {
		char num[128];
		if(i && i % 4 == 0)
			strcat(s_ret, "\n");
		sprintf(num, "%f ", m[i]);
		strcat(s_ret, num);
	}
};

int geom_vector3_matrix4_mul(const Real* vec3, const Real* mat4, Real* vec3_ret) {
	Real vec4[4] = {vec3[0], vec3[1], vec3[2], 1};					
	Real vec4_result[4];
	geom_vector4_matrix4_mul(vec4, mat4, vec4_result);
	int i;
	for(i = 0; i < 3; ++i) {
		vec3_ret[i] = vec4_result[i];
	}
	return GEOM_NO_ERROR;
}

int geom_vector4_matrix4_mul(const Real* v, const Real* m, Real* vec4_ret) {
	vec4_ret[0] = v[0]*m[ 0] + v[1]*m[ 4] + v[2]*m[ 8] + v[3]*m[ 12];
	vec4_ret[1] = v[0]*m[ 1] + v[1]*m[ 5] + v[2]*m[ 9] + v[3]*m[ 13];
	vec4_ret[2] = v[0]*m[ 2] + v[1]*m[ 6] + v[2]*m[10] + v[3]*m[ 14];
	vec4_ret[3] = v[0]*m[ 3] + v[1]*m[ 7] + v[2]*m[11] + v[3]*m[ 15];
	return GEOM_NO_ERROR;
}

