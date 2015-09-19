#ifndef MATRIX_HPP
# define MATRIX_HPP

/*
 * Some extra matrix, vector and quaternion functions.
 *
 * Many of these already exist in some variant in assimp,
 * but not exported to the C interface.
 */

// convert 4x4 to column major format for opengl
void transposematrix(float m[16], aiMatrix4x4 *p);
void extract3x3(aiMatrix3x3 *m3, aiMatrix4x4 *m4);
void mixvector(aiVector3D *p, aiVector3D *a, aiVector3D *b, float t);
float dotquaternions(aiQuaternion *a, aiQuaternion *b);
void normalizequaternion(aiQuaternion *q);
void mixquaternion(aiQuaternion *q, aiQuaternion *a, aiQuaternion *b, float t);
void composematrix(aiMatrix4x4 *m,
	aiVector3D *t, aiQuaternion *q, aiVector3D *s);

#endif
