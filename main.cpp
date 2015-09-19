
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

// int vertexcount = 0, facecount = 0; // for statistics only

// opengl (and skinned vertex) buffers for the meshes
int meshcount = 0;


#include <maths/matrix.hpp>
#include <texturesLoading/texturesLoading.hpp>
#include <scene/scene.hpp>
#include <render/render.hpp>

extern struct mesh *meshlist;
#include <graphic.h>
GLFWwindow		*window;

#define ISOMETRIC 35.264	// true isometric view

struct aiScene *g_scene = NULL;
int lasttime = 0;
struct aiAnimation *curanim = NULL;
int animfps = 30, animlen = 0;
float animtick = 0;
int playing = 1;

// int showhelp = 0;
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
	static double	old = glfwGetTime();
	double			tmp;
	switch (key) {
		case 27: case 'q': exit(1); break;
	}

	if (playing) {
		tmp = glfwGetTime();
		lasttime = tmp - old;
		old = tmp;
	}
	// 	lasttime = glutGet(GLUT_ELAPSED_TIME);
}

#define PATH "ressources/ArmyPilot.dae"

static int		initGlfw(void)
{
		// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.1
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	window = glfwCreateWindow( 1024, 768, "Bomberman 3D", NULL, NULL); 
	if( window == NULL )
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW 
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// glfwSetKeyCallback(window, key_callback);
	glEnable(GL_DEPTH_TEST);
	return (0);
}

int main(int ac, char **av)
{
	initGlfw();
	float clearcolor[4] = { 0.22, 0.22, 0.22, 1.0 };
	float zoom = 1;
	// int c;

	// glutInitWindowPosition(50, 50+24);
	// glutInitWindowSize(screenw, screenh);
	// glutInit(&ac, av);
	// glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	// while ((c = getopt(argc, argv, "iIgtawblc:r:p:z:f:")) != -1) {
	// }

	// glutCreateWindow("Bomberman 3D");
	// screenw = glutGet(GLUT_WINDOW_WIDTH);
	// screenh = glutGet(GLUT_WINDOW_HEIGHT);

	initchecker();

	int flags = aiProcess_Triangulate;
	flags |= aiProcess_JoinIdenticalVertices;
	flags |= aiProcess_GenSmoothNormals;
	flags |= aiProcess_GenUVCoords;
	flags |= aiProcess_TransformUVCoords;
	flags |= aiProcess_RemoveComponent;

	strcpy(basedir, PATH);
	char *p = strrchr(basedir, '/');
	if (!p) p = strrchr(basedir, '\\');
	if (!p) strcpy(basedir, ""); else p[1] = 0;

	g_scene = (struct aiScene*) aiImportFile(PATH, flags);
	if (g_scene) {
		initscene(g_scene);

		float radius = measurescene(g_scene, camera.center);
		camera.distance = radius * 2 * zoom;
		gridsize = (int)radius + 1;
		mindist = radius * 0.1;
		maxdist = radius * 10;

		setanim(0);
	} else {
		fprintf(stderr, "cannot import scene: '%s'\n", PATH);
	}

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glClearColor(clearcolor[0], clearcolor[1], clearcolor[2], clearcolor[3]);
	while (42) {
		display();
	}
		// glutReshapeFunc(reshape);
		// glutKeyboardFunc(keyboard);
		// glutDisplayFunc(display);
		// glutMainLoop();
	}