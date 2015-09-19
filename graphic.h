#ifndef GRAPHIC_H
	# ifdef __APPLE__
	#  define __gl_h_
	#  define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
	#  include <OpenGL/OpenGL.h>
	#  include <OpenGL/gl3.h>
	# else
	#  include <GL/gl.h>
	# endif
	
	# include <GLFW/glfw3.h>

	GLuint		LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
	int			onwork(void);

#endif
	