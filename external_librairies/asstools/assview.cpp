#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/freeglut.h>
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

char basedir[2000];
unsigned int checker_texture = 0;

#include <maths/matrix.hpp>
#include <texturesLoading/texturesLoading.hpp>

/*
 * Init, animate and draw aiScene.
 *
 * We build our own list of the meshes to keep the vertex data
 * in a format suitable for use with OpenGL. We also store the
 * resulting meshes during skeletal animation here.
 */

int vertexcount = 0, facecount = 0; // for statistics only

// opengl (and skinned vertex) buffers for the meshes
int meshcount = 0;
struct mesh {
	struct aiMesh *mesh;
	unsigned int texture;
	int vertexcount, elementcount;
	float *position;
	float *normal;
	float *texcoord;
	int *element;
} *meshlist = NULL;

// find a node by name in the hierarchy (for anims and bones)
struct aiNode *findnode(struct aiNode *node, char *name)
{
	int i;
	if (!strcmp(name, node->mName.data))
		return node;
	for (i = 0; i < node->mNumChildren; i++) {
		struct aiNode *found = findnode(node->mChildren[i], name);
		if (found)
			return found;
	}
	return NULL;
}

// calculate absolute transform for node to do mesh skinning
void transformnode(aiMatrix4x4 *result, struct aiNode *node)
{
	if (node->mParent) {
		transformnode(result, node->mParent);
		aiMultiplyMatrix4(result, &node->mTransformation);
	} else {
		*result = node->mTransformation;
	}
}

void transformmesh(struct aiScene *scene, struct mesh *mesh)
{
	struct aiMesh *amesh = mesh->mesh;
	aiMatrix4x4 skin4;
	aiMatrix3x3 skin3;
	int i, k;

	if (amesh->mNumBones == 0)
		return;

	memset(mesh->position, 0, mesh->vertexcount * 3 * sizeof(float));
	memset(mesh->normal, 0, mesh->vertexcount * 3 * sizeof(float));

	for (k = 0; k < amesh->mNumBones; k++) {
		struct aiBone *bone = amesh->mBones[k];
		struct aiNode *node = findnode(scene->mRootNode, bone->mName.data);

		transformnode(&skin4, node);
		aiMultiplyMatrix4(&skin4, &bone->mOffsetMatrix);
		extract3x3(&skin3, &skin4);

		for (i = 0; i < bone->mNumWeights; i++) {
			int v = bone->mWeights[i].mVertexId;
			float w = bone->mWeights[i].mWeight;

			aiVector3D position = amesh->mVertices[v];
			aiTransformVecByMatrix4(&position, &skin4);
			mesh->position[v*3+0] += position.x * w;
			mesh->position[v*3+1] += position.y * w;
			mesh->position[v*3+2] += position.z * w;

			aiVector3D normal = amesh->mNormals[v];
			aiTransformVecByMatrix3(&normal, &skin3);
			mesh->normal[v*3+0] += normal.x * w;
			mesh->normal[v*3+1] += normal.y * w;
			mesh->normal[v*3+2] += normal.z * w;
		}
	}
}

void initmesh(struct aiScene *scene, struct mesh *mesh, struct aiMesh *amesh)
{
	int i;

	vertexcount += amesh->mNumVertices;
	facecount += amesh->mNumFaces;

	mesh->mesh = amesh; // stow away pointer for bones

	mesh->texture = loadmaterial(scene->mMaterials[amesh->mMaterialIndex]);

	mesh->vertexcount = amesh->mNumVertices;
	mesh->position = (float *)calloc(mesh->vertexcount * 3, sizeof(float));
	mesh->normal = (float *)calloc(mesh->vertexcount * 3, sizeof(float));
	mesh->texcoord = (float *)calloc(mesh->vertexcount * 2, sizeof(float));

	for (i = 0; i < mesh->vertexcount; i++) {
		mesh->position[i*3+0] = amesh->mVertices[i].x;
		mesh->position[i*3+1] = amesh->mVertices[i].y;
		mesh->position[i*3+2] = amesh->mVertices[i].z;

		if (amesh->mNormals) {
			mesh->normal[i*3+0] = amesh->mNormals[i].x;
			mesh->normal[i*3+1] = amesh->mNormals[i].y;
			mesh->normal[i*3+2] = amesh->mNormals[i].z;
		}

		if (amesh->mTextureCoords[0]) {
			mesh->texcoord[i*2+0] = amesh->mTextureCoords[0][i].x;
			mesh->texcoord[i*2+1] = 1 - amesh->mTextureCoords[0][i].y;
		}
	}

	mesh->elementcount = amesh->mNumFaces * 3;
	mesh->element = (int *)calloc(mesh->elementcount, sizeof(int));

	for (i = 0; i < amesh->mNumFaces; i++) {
		struct aiFace *face = amesh->mFaces + i;
		mesh->element[i*3+0] = face->mIndices[0];
		mesh->element[i*3+1] = face->mIndices[1];
		mesh->element[i*3+2] = face->mIndices[2];
	}
}

void initscene(struct aiScene *scene)
{
	int i;
	meshcount = scene->mNumMeshes;
	meshlist = (struct mesh *)calloc(meshcount, sizeof (*meshlist));
	for (i = 0; i < meshcount; i++) {
		initmesh(scene, meshlist + i, scene->mMeshes[i]);
		transformmesh(scene, meshlist + i);
	}
}

void drawmesh(struct mesh *mesh)
{
	if (mesh->texture > 0) {
		glColor4f(1, 1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
	} else {
		glColor4f(0.9, 0.7, 0.7, 1);
		glBindTexture(GL_TEXTURE_2D, checker_texture);
	}
	glVertexPointer(3, GL_FLOAT, 0, mesh->position);
	glNormalPointer(GL_FLOAT, 0, mesh->normal);
	glTexCoordPointer(2, GL_FLOAT, 0, mesh->texcoord);
	glDrawElements(GL_TRIANGLES, mesh->elementcount, GL_UNSIGNED_INT, mesh->element);
}

void drawnode(aiNode *node, aiMatrix4x4 world)
{
	float mat[16];
	int i;

	aiMultiplyMatrix4(&world, &node->mTransformation);
	transposematrix(mat, &world);

	for (i = 0; i < node->mNumMeshes; i++) {
		struct mesh *mesh = meshlist + node->mMeshes[i];
		if (mesh->mesh->mNumBones == 0) {
			// non-skinned meshes are in node-local space
			glPushMatrix();
			glMultMatrixf(mat);
			drawmesh(mesh);
			glPopMatrix();
		} else {
			// skinned meshes are already in world space
			drawmesh(mesh);
		}
	}

	for (i = 0; i < node->mNumChildren; i++)
		drawnode(node->mChildren[i], world);
}

void drawscene(struct aiScene *scene)
{
	aiMatrix4x4 world;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	aiIdentityMatrix4(&world);
	drawnode(scene->mRootNode, world);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void measuremesh(struct mesh *mesh, aiMatrix4x4 transform, float bboxmin[3], float bboxmax[3])
{
	aiVector3D p;
	int i;
	for (i = 0; i < mesh->vertexcount; i++) {
		p.x = mesh->position[i*3+0];
		p.y = mesh->position[i*3+1];
		p.z = mesh->position[i*3+2];
		aiTransformVecByMatrix4(&p, &transform);
		bboxmin[0] = MIN(bboxmin[0], p.x);
		bboxmin[1] = MIN(bboxmin[1], p.y);
		bboxmin[2] = MIN(bboxmin[2], p.z);
		bboxmax[0] = MAX(bboxmax[0], p.x);
		bboxmax[1] = MAX(bboxmax[1], p.y);
		bboxmax[2] = MAX(bboxmax[2], p.z);
	}
}

void measurenode(aiNode *node, aiMatrix4x4 world, float bboxmin[3], float bboxmax[3])
{
	aiMatrix4x4 identity;
	int i;

	aiMultiplyMatrix4(&world, &node->mTransformation);
	aiIdentityMatrix4(&identity);

	for (i = 0; i < node->mNumMeshes; i++) {
		struct mesh *mesh = meshlist + node->mMeshes[i];
		if (mesh->mesh->mNumBones == 0) {
			// non-skinned meshes are in node-local space
			measuremesh(mesh, world, bboxmin, bboxmax);
		} else {
			// skinned meshes are already in world space
			measuremesh(mesh, identity, bboxmin, bboxmax);
		}
	}

	for (i = 0; i < node->mNumChildren; i++)
		measurenode(node->mChildren[i], world, bboxmin, bboxmax);
}

float measurescene(struct aiScene *scene, float center[3])
{
	aiMatrix4x4 world;
	float bboxmin[3];
	float bboxmax[3];
	float dx, dy, dz;

	bboxmin[0] = 1e10; bboxmax[0] = -1e10;
	bboxmin[1] = 1e10; bboxmax[1] = -1e10;
	bboxmin[2] = 1e10; bboxmax[2] = -1e10;

	aiIdentityMatrix4(&world);
	measurenode(scene->mRootNode, world, bboxmin, bboxmax);

	center[0] = (bboxmin[0] + bboxmax[0]) / 2;
	center[1] = (bboxmin[1] + bboxmax[1]) / 2;
	center[2] = (bboxmin[2] + bboxmax[2]) / 2;

	dx = MAX(center[0] - bboxmin[0], bboxmax[0] - center[0]);
	dy = MAX(center[1] - bboxmin[1], bboxmax[1] - center[1]);
	dz = MAX(center[2] - bboxmin[2], bboxmax[2] - center[2]);

	return sqrt(dx*dx + dy*dy + dz*dz);
}

int animationlength(struct aiAnimation *anim)
{
	int i, len = 0;
	for (i = 0; i < anim->mNumChannels; i++) {
		struct aiNodeAnim *chan = anim->mChannels[i];
		len = MAX(len, chan->mNumPositionKeys);
		len = MAX(len, chan->mNumRotationKeys);
		len = MAX(len, chan->mNumScalingKeys);
	}
	return len;
}

void animatescene(struct aiScene *scene, struct aiAnimation *anim, float tick)
{
	struct aiVectorKey *p0, *p1, *s0, *s1;
	struct aiQuatKey *r0, *r1;
	aiVector3D p, s;
	aiQuaternion r;
	int i;

	// Assumes even key frame rate and synchronized pos/rot/scale keys.
	// We should look at the key->mTime values instead, but I'm lazy.

	int frame = floor(tick);
	float t = tick - floor(tick);

	for (i = 0; i < anim->mNumChannels; i++) {
		struct aiNodeAnim *chan = anim->mChannels[i];
		struct aiNode *node = findnode(scene->mRootNode, chan->mNodeName.data);
		p0 = chan->mPositionKeys + (frame+0) % chan->mNumPositionKeys;
		p1 = chan->mPositionKeys + (frame+1) % chan->mNumPositionKeys;
		r0 = chan->mRotationKeys + (frame+0) % chan->mNumRotationKeys;
		r1 = chan->mRotationKeys + (frame+1) % chan->mNumRotationKeys;
		s0 = chan->mScalingKeys + (frame+0) % chan->mNumScalingKeys;
		s1 = chan->mScalingKeys + (frame+1) % chan->mNumScalingKeys;
		mixvector(&p, &p0->mValue, &p1->mValue, t);
		mixquaternion(&r, &r0->mValue, &r1->mValue, t);
		mixvector(&s, &s0->mValue, &s1->mValue, t);
		composematrix(&node->mTransformation, &p, &r, &s);
	}

	for (i = 0; i < meshcount; i++)
		transformmesh(scene, meshlist + i);
}

/*
 * Boring UI and GLUT hooks.
 */

#include "getopt.c"

#define ISOMETRIC 35.264	// true isometric view
#define DIMETRIC 30		// 2:1 'isometric' as seen in pixel art

struct aiScene *g_scene = NULL;
int lasttime = 0;
struct aiAnimation *curanim = NULL;
int animfps = 30, animlen = 0;
float animtick = 0;
int playing = 1;

int showhelp = 0;
int doplane = 0;
int doalpha = 0;
int dowire = 0;
int dotexture = 1;
int dobackface = 1;
int dotwosided = 1;
int doperspective = 1;

int screenw = 800, screenh = 600;
int mousex, mousey, mouseleft = 0, mousemiddle = 0, mouseright = 0;

int gridsize = 3;
float mindist = 1;
float maxdist = 10;

float light_position[4] = { -1, 2, 2, 0 };

struct {
	float distance;
	float yaw;
	float pitch;
	float center[3];
} camera = { 3, 45, -DIMETRIC, { 0, 1, 0 } };

void setanim(int i)
{
	if (!g_scene) return;
	if (g_scene->mNumAnimations == 0) return;
	i = MIN(i, g_scene->mNumAnimations - 1);
	curanim = g_scene->mAnimations[i];
	animlen = animationlength(curanim);
	animfps = 30;
	animtick = 0;
	if (animfps < 1)
		animfps = 30;
}

void perspective(float fov, float aspect, float znear, float zfar)
{
	fov = fov * 3.14159 / 360.0;
	fov = tan(fov) * znear;
	glFrustum(-fov * aspect, fov * aspect, -fov, fov, znear, zfar);
}

// void orthogonal(float fov, float aspect, float znear, float zfar)
// {
// 	glOrtho(-fov * aspect, fov * aspect, -fov, fov, znear, zfar);
// }

// void drawstring(float x, float y, char *s)
// {
// 	glRasterPos2f(x+0.375, y+0.375);
// 	while (*s)
// 		//glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s++);
// 		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s++);
// }

																															// void mouse(int button, int state, int x, int y)
																															// {
																															// 	if (button == GLUT_LEFT_BUTTON) mouseleft = state == GLUT_DOWN;
																															// 	if (button == GLUT_MIDDLE_BUTTON) mousemiddle = state == GLUT_DOWN;
																															// 	if (button == GLUT_RIGHT_BUTTON) mouseright = state == GLUT_DOWN;
																															// 	mousex = x;
																															// 	mousey = y;
																															// }

																															// void motion(int x, int y)
																															// {
																															// 	int dx = x - mousex;
																															// 	int dy = y - mousey;
																															// 	if (mouseleft) {
																															// 		camera.yaw -= dx * 0.3;
																															// 		camera.pitch -= dy * 0.2;
																															// 		if (camera.pitch < -85) camera.pitch = -85;
																															// 		if (camera.pitch > 85) camera.pitch = 85;
																															// 		if (camera.yaw < 0) camera.yaw += 360;
																															// 		if (camera.yaw > 360) camera.yaw -= 360;
																															// 	}
																															// 	if (mousemiddle || mouseright) {
																															// 		camera.distance += dy * 0.01 * camera.distance;
																															// 		if (camera.distance < mindist) camera.distance = mindist;
																															// 		if (camera.distance > maxdist) camera.distance = maxdist;
																															// 	}
																															// 	mousex = x;
																															// 	mousey = y;
																															// 	glutPostRedisplay();
																															// }

// void togglefullscreen(void)
// {
// 	static int oldw = 100, oldh = 100;
// 	static int oldx = 0, oldy = 0;
// 	static int isfullscreen = 0;
// 	if (!isfullscreen) {
// 		oldw = screenw;
// 		oldh = screenh;
// 		oldx = glutGet(GLUT_WINDOW_X);
// 		oldy = glutGet(GLUT_WINDOW_Y);
// 		glutFullScreen();
// 	} else {
// 		glutPositionWindow(oldx, oldy);
// 		glutReshapeWindow(oldw, oldh);
// 	}
// 	isfullscreen = !isfullscreen;
// }

																															void keyboard(unsigned char key, int x, int y)
																															{
																																switch (key) {
																																case 27: case 'q': exit(1); break;
																																// case 'h': case '?': showhelp = !showhelp; break;
																																// case 'f': togglefullscreen(); break;
																																// case 'i': doperspective = 0; camera.yaw = 45; camera.pitch = -DIMETRIC; break;
																																// case 'I': doperspective = 0; camera.yaw = 45; camera.pitch = -ISOMETRIC; break;
																																// case 'p': doperspective = !doperspective; break;
																																// case '0': animtick = 0; animfps = 30; break;
																																// case '1': case '2': case '3': case '4':
																																// case '5': case '6': case '7': case '8':
																																// case '9': setanim(key - '1'); break;
																																// case ' ': playing = !playing; break;
																																// case '.': animtick = floor(animtick) + 1; break;
																																// case ',': animtick = floor(animtick) - 1; break;
																																// case '[': animfps = MAX(5, animfps-5); break;
																																// case ']': animfps = MIN(60, animfps+5); break;
																																// case 'g': doplane = !doplane; break;
																																// case 't': dotexture = !dotexture; break;
																																// case 'A': doalpha--; break;
																																// case 'a': doalpha++; break;
																																// case 'w': dowire = !dowire; break;
																																// case 'b': dobackface = !dobackface; break;
																																// case 'l': dotwosided = !dotwosided; break;
																																}

																																if (playing)
																																	lasttime = glutGet(GLUT_ELAPSED_TIME);

																																// glutPostRedisplay();
																															}

// void special(int key, int x, int y)
// {
// 	switch (key) {
// 	case GLUT_KEY_F4: exit(1); break;
// 	case GLUT_KEY_F1: showhelp = !showhelp; break;
// 	}
	// glutPostRedisplay();
// }

																																void reshape(int w, int h)
																																{
																																	screenw = w;
																																	screenh = h;
																																	glViewport(0, 0, w, h);
																																}

void display(void)
{
	char buf[256];
	int time, timestep;
	int i;

	time = glutGet(GLUT_ELAPSED_TIME);
	timestep = time - lasttime;
	lasttime = time;

	if (g_scene) {
		if (curanim) {
			if (playing) {
				animtick = animtick + (timestep/1000.0) * animfps;
				glutPostRedisplay();
			}
			while (animtick < 0) animtick += animlen;
			while (animtick >= animlen) animtick -= animlen;
			animatescene(g_scene, curanim, animtick);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// if (doperspective)
		perspective(50, (float)screenw/screenh, mindist/5, maxdist*5);
	// else
	// 	orthogonal(camera.distance/2, (float)screenw/screenh, mindist/5, maxdist*5);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glTranslatef(0, 0, -camera.distance);
	glRotatef(-camera.pitch, 1, 0, 0);
	glRotatef(-camera.yaw, 0, 1, 0);
	glTranslatef(-camera.center[0], -camera.center[1], -camera.center[2]);

	if (dotexture)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	if (dowire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (dobackface)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, dotwosided);

	doalpha = CLAMP(doalpha, 0, 4);
	switch (doalpha) {
	// No alpha transparency.
	case 0:
		if (g_scene) drawscene(g_scene);
		break;

	// Alpha test only. Always correct, but aliased and ugly.
	case 1:
		glAlphaFunc(GL_GREATER, 0.2);
		glEnable(GL_ALPHA_TEST);
		if (g_scene) drawscene(g_scene);
		glDisable(GL_ALPHA_TEST);
		break;

	// Quick-and-dirty hack: render with both test and blend.
	// Background may leak through depending on drawing order.
	case 2:
		glAlphaFunc(GL_GREATER, 0.2);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		if (g_scene) drawscene(g_scene);
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		break;

	// For best looking alpha blending, render twice.
	// Solid parts first to fill the depth buffer.
	// Transparent parts after, with z-write disabled.
	// Background is safe, but internal blend order may be wrong.
	case 3:
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_EQUAL, 1);
		if (g_scene) drawscene(g_scene);

		glAlphaFunc(GL_LESS, 1);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		if (g_scene) drawscene(g_scene);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		break;

	// If we have a multisample buffer, we can get 'perfect' transparency
	// by using alpha-as-coverage. This does have a few limitations, depending
	// on the number of samples available you'll get banding or dithering artefacts.
	case 4:
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		if (g_scene) drawscene(g_scene);
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		break;
	}

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	if (doplane) {
		glBegin(GL_LINES);
		glColor4f(0.4, 0.4, 0.4, 1);
		for (i = -gridsize; i <= gridsize; i ++) {
			glVertex3f(i, 0, -gridsize); glVertex3f(i, 0, gridsize);
			glVertex3f(-gridsize, 0, i); glVertex3f(gridsize, 0, i);
		}
		glEnd();
	}

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screenw, screenh, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1, 1, 1, 1);
	if (g_scene) {
		sprintf(buf, "%d meshes; %d vertices; %d faces ", meshcount, vertexcount, facecount);
		// drawstring(8, 18+0, buf);
		if (curanim) {
			sprintf(buf, "frame %03d / %03d (%d fps)", (int)animtick+1, animlen, animfps);
			// drawstring(8, 18+20, buf);
		}
	} else {
		// drawstring(8, 18, "No model loaded!");
	}

	if (showhelp) {
		#define Y(n) 18+40+n*16
		glColor4f(1, 1, 0.5, 1);
		// drawstring(8, Y(0), "a - change transparency mode");
		// drawstring(8, Y(1), "t - toggle textures");
		// drawstring(8, Y(2), "w - toggle wireframe");
		// drawstring(8, Y(3), "b - toggle backface culling");
		// drawstring(8, Y(4), "l - toggle two-sided lighting");
		// drawstring(8, Y(5), "g - toggle ground plane");
		// drawstring(8, Y(6), "p - toggle orthogonal/perspective camera");
		// drawstring(8, Y(7), "i - set up dimetric camera (2:1)");
		// drawstring(8, Y(8), "I - set up isometric camera");

		if (1|| curanim) {
			// drawstring(8, Y(10), "space - play/pause animation");
			// drawstring(8, Y(11), "[ and ] - change animation playback speed");
			// drawstring(8, Y(12), ", and . - single step animation");
		}
	}

	glutSwapBuffers();

	i = glGetError();
	if (i) fprintf(stderr, "opengl error: %d\n", i);
}

void usage(void)
{
	fprintf(stderr, "usage: assview [-geometry WxH] [options] asset.dae\n");
	fprintf(stderr, "\t-i\tdimetric (2:1) camera\n");
	fprintf(stderr, "\t-I\ttrue isometric camera\n");
	fprintf(stderr, "\t-a\talpha transparency mode; use more times for higher quality.\n");
	fprintf(stderr, "\t-b\tdon't render backfaces\n");
	fprintf(stderr, "\t-g\trender ground plane\n");
	fprintf(stderr, "\t-l\tone-sided lighting\n");
	fprintf(stderr, "\t-t\tdon't render textures\n");
	fprintf(stderr, "\t-w\trender wireframe\n");
	fprintf(stderr, "\t-c r,g,b\tbackground color\n");
	fprintf(stderr, "\t-r n\trotate camera n degrees (yaw)\n");
	fprintf(stderr, "\t-p n\tpitch camera n degrees\n");
	fprintf(stderr, "\t-z n\tzoom camera n times\n");
	fprintf(stderr, "\t-f n\trender animation at frame n\n");
	exit(1);
}

int main(int argc, char **argv)
{
	float clearcolor[4] = { 0.22, 0.22, 0.22, 1.0 };
	float zoom = 1;
	int c;

	glutInitWindowPosition(50, 50+24);
	glutInitWindowSize(screenw, screenh);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	while ((c = getopt(argc, argv, "iIgtawblc:r:p:z:f:")) != -1) {
		switch (c) {
		case 'i': doperspective = 0; camera.yaw = 45; camera.pitch = -DIMETRIC; break;
		case 'I': doperspective = 0; camera.yaw = 45; camera.pitch = -ISOMETRIC; break;
		case 'g': doplane = 1; break;
		case 't': dotexture = 0; break;
		case 'a': doalpha++; break;
		case 'w': dowire = 1; break;
		case 'b': dobackface = 0; break;
		case 'l': dotwosided = 0; break;
		case 'c': sscanf(optarg, "%g,%g,%g", clearcolor+0, clearcolor+1, clearcolor+2); break;
		case 'r': camera.yaw = atof(optarg); break;
		case 'p': camera.pitch = atof(optarg); break;
		case 'z': zoom = atof(optarg); break;
		case 'f': animtick = atof(optarg); break;
		default: usage(); break;
		}
	}

	glutCreateWindow("Asset Viewer");
	screenw = glutGet(GLUT_WINDOW_WIDTH);
	screenh = glutGet(GLUT_WINDOW_HEIGHT);

// #ifdef __APPLE__
	// int one = 1;
	// void *ctx = CGLGetCurrentContext();
	// CGLSetParameter(ctx, kCGLCPSwapInterval, &one);
// #endif

	initchecker();

	if (optind < argc) {
		int flags = aiProcess_Triangulate;
		flags |= aiProcess_JoinIdenticalVertices;
		flags |= aiProcess_GenSmoothNormals;
		flags |= aiProcess_GenUVCoords;
		flags |= aiProcess_TransformUVCoords;
		flags |= aiProcess_RemoveComponent;

		strcpy(basedir, argv[1]);
		char *p = strrchr(basedir, '/');
		if (!p) p = strrchr(basedir, '\\');
		if (!p) strcpy(basedir, ""); else p[1] = 0;

		glutSetWindowTitle(argv[1]);

		g_scene = (struct aiScene*) aiImportFile(argv[optind], flags);
		if (g_scene) {
			initscene(g_scene);

			float radius = measurescene(g_scene, camera.center);
			camera.distance = radius * 2 * zoom;
			gridsize = (int)radius + 1;
			mindist = radius * 0.1;
			maxdist = radius * 10;

			setanim(0);
		} else {
			fprintf(stderr, "cannot import scene: '%s'\n", argv[1]);
		}
	}

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glClearColor(clearcolor[0], clearcolor[1], clearcolor[2], clearcolor[3]);

																																						glutReshapeFunc(reshape);
																						glutDisplayFunc(display);
																																						// glutMouseFunc(mouse);
																																						// glutMotionFunc(motion);
																																						glutKeyboardFunc(keyboard);
																																						// glutSpecialFunc(special);
																						glutMainLoop();

	return 0;
}
