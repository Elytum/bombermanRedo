#ifndef SCENE_HPP
# define SCENE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
// #include <GLUT/glut.h>
#else
#include <GL/gl.h>
// #include <GL/freeglut.h>
#endif

#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#ifndef GL_SAMPLE_ALPHA_TO_COVERAGE
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#endif

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CLAMP(x,a,b) MIN(MAX(x,a),b)

// char basedir[2000];
// unsigned int checker_texture = 0;

// #include <maths/matrix.hpp>
// #include <texturesLoading/texturesLoading.hpp>
// #include <scene/scene.hpp>


/*
 * Init, animate and draw aiScene.
 *
 * We build our own list of the meshes to keep the vertex data
 * in a format suitable for use with OpenGL. We also store the
 * resulting meshes during skeletal animation here.
 */

// int vertexcount = 0, facecount = 0; // for statistics only

// opengl (and skinned vertex) buffers for the meshes
// int meshcount = 0;
// struct mesh {
// 	struct aiMesh *mesh;
// 	unsigned int texture;
// 	int vertexcount, elementcount;
// 	float *position;
// 	float *normal;
// 	float *texcoord;
// 	int *element;
// } *meshlist = NULL;

struct aiNode	*findnode(struct aiNode *node, char *name);
void			transformnode(aiMatrix4x4 *result, struct aiNode *node);
void			transformmesh(struct aiScene *scene, struct mesh *mesh);
void			initmesh(struct aiScene *scene, struct mesh *mesh, struct aiMesh *amesh);
void			initscene(struct aiScene *scene);
void			drawmesh(struct mesh *mesh);
void			drawnode(aiNode *node, aiMatrix4x4 world);
void			drawscene(struct aiScene *scene);
void			measuremesh(struct mesh *mesh, aiMatrix4x4 transform, float bboxmin[3], float bboxmax[3]);
void			measurenode(aiNode *node, aiMatrix4x4 world, float bboxmin[3], float bboxmax[3]);
float			measurescene(struct aiScene *scene, float center[3]);
int				animationlength(struct aiAnimation *anim);
void			animatescene(struct aiScene *scene, struct aiAnimation *anim, float tick);

#endif
