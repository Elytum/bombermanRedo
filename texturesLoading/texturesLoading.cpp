


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

#include <maths/matrix.hpp>

#define STBI_NO_HDR

#include <stb_image/stb_image.c>


extern char basedir[2000];
extern unsigned int checker_texture;

unsigned char checker_data[256*256];

void initchecker(void)
{
	int x, y, i = 0;
	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			int k = ((x>>5) & 1) ^ ((y>>5) & 1);
			checker_data[i++] = k ? 255 : 192;
		}
	}
	glGenTextures(1, &checker_texture);
	glBindTexture(GL_TEXTURE_2D, checker_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, checker_data);
}

void lowerstring(char *s)
{
	while (*s) { *s = tolower(*s); s++; }
}

unsigned int loadtexture(char *filename)
{
	unsigned int texture;
	unsigned char *image;
	int w, h, n, intfmt = 0, fmt = 0;

	image = stbi_load(filename, &w, &h, &n, 0);
	if (!image) {
		lowerstring(filename);
		image = stbi_load(filename, &w, &h, &n, 0);
		if (!image) {
			fprintf(stderr, "cannot load texture '%s'\n", filename);
			return 0;
		}
	}

	if (n == 1) { intfmt = fmt = GL_LUMINANCE; }
	if (n == 2) { intfmt = fmt = GL_LUMINANCE_ALPHA; }
	if (n == 3) { intfmt = fmt = GL_RGB; }
	if (n == 4) { intfmt = fmt = GL_RGBA; }

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, intfmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D);

	free(image);

	return texture;
}

unsigned int loadmaterial(struct aiMaterial *material)
{
	char filename[2000];
	struct aiString str;
	if (!aiGetMaterialString(material, AI_MATKEY_TEXTURE_DIFFUSE(0), &str)) {
		dprintf(1, "Condition ok\n");
		char *s = strrchr(str.data, '/');
		if (!s) s = strrchr(str.data, '\\');
		if (!s) s = str.data; else s++;
		strcpy(filename, basedir);
		strcat(filename, s);
		dprintf(1, "loading [%s]\n", filename);
		return loadtexture(filename);
	}
	return 0;
}
