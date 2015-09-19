#include <Model/Model.hpp>

Model::Model(void)	{
}

Model::Model(GLuint vaoID, GLsizei vertexCount) :	vaoID(vaoID), vertexCount(vertexCount)	{
}

Model::Model( Model const & cpy)	{
	*this = cpy;
}

Model::~Model() {

}

Model&		Model::operator=(Model const & cpy)	{
	vaoID = cpy.vaoID;
	vertexCount = cpy.vertexCount;
	return *this;
}

GLuint		Model::getVaoID(void)	{
	return (vaoID);
}

GLsizei		Model::getVertexCount(void)	{
	return (vertexCount);
}