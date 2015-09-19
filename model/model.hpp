#ifndef MODEL_HPP
# define MODEL_HPP

#include <graphic.h>

class Model
{

	public:
		Model(void);
		Model(GLuint vaoID, GLsizei vertexCount);
		~Model();
		Model( Model const & cpy);
		Model&		operator=(Model const & cpy);
		GLuint		getVaoID(void);
		GLsizei		getVertexCount(void);

	private:
		GLuint		vaoID;
		GLsizei		vertexCount;
};

#endif
