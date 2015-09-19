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

extern struct mesh *meshlist;

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
