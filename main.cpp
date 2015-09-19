
#define DIMETRIC 30		// 2:1 'isometric' as seen in pixel art

struct {
	float distance;
	float yaw;
	float pitch;
	float center[3];
} camera = { 3, 45, -DIMETRIC, { 0, 1, 0 } };

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

int vertexcount = 0, facecount = 0; // for statistics only

// opengl (and skinned vertex) buffers for the meshes
int meshcount = 0;


#include <maths/matrix.hpp>
#include <texturesLoading/texturesLoading.hpp>
#include <scene/scene.hpp>
#include <render/render.hpp>

extern struct mesh *meshlist;

/*
 * Boring UI and GLUT hooks.
 */

#include "external_librairies/getopt/getopt.c"

#define ISOMETRIC 35.264	// true isometric view

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
		}
	}

	glutCreateWindow("Asset Viewer");
	screenw = glutGet(GLUT_WINDOW_WIDTH);
	screenh = glutGet(GLUT_WINDOW_HEIGHT);

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
		// glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutDisplayFunc(display);
		glutMainLoop();
	}