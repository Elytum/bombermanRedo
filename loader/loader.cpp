#include <Loader/Loader.hpp>

Loader::Loader(void) {
}

Loader::~Loader() {

}

Loader::Loader( Loader const & cpy) {
}

Loader&			Loader::operator=(Loader const & cpy) {

	return *this;
}

void			Loader::cleanUp(void)
{
	std::list<GLuint>::const_iterator iterator;

	for (iterator = vaos.begin(); iterator != vaos.end(); ++iterator) {
		glDeleteVertexArrays(1, &(*iterator));
	}
	for (iterator = vbos.begin(); iterator != vbos.end(); ++iterator) {
		glDeleteVertexArrays(1, &(*iterator));
	}
	for (iterator = textures.begin(); iterator != textures.end(); ++iterator) {
		glDeleteTextures(1, &(*iterator));
	}
}

GLuint			Loader::createVAO(void)
{
	GLuint					vaoID;

	glGenVertexArrays (1, &vaoID);
	vaos.push_front(vaoID);
	glBindVertexArray (vaoID);
	return (vaoID);
}

void			Loader::unbindVAO(void)	{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint			Loader::storeDataInAttributeList(int attribute,
					int coordsize, GLvoid *data, GLsizeiptr len)
{
	GLuint			vboID;

	glGenBuffers (1, &vboID);
	vbos.push_front(vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, len * sizeof(float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(attribute, coordsize, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return (vboID);
}

GLuint			Loader::bindIndicesBuffer(GLuint *indices, GLsizeiptr len)
{
	GLuint			vboID;

	glGenBuffers (1, &vboID);
	vbos.push_front(vboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		len * sizeof(GLuint), indices, GL_STATIC_DRAW);
	return (vboID);
}

Model			*Loader::loadToVAO(	float positions[], GLsizeiptr nbpos,
									float texture[], GLsizeiptr nbtex,
									GLuint indices[], GLsizeiptr nbind)	{
	int		vaoID = createVAO();

	storeDataInAttributeList(0, 3, positions, nbpos);
	storeDataInAttributeList(1, 2, texture, nbtex);
	bindIndicesBuffer(indices, nbind);
	unbindVAO();
	return new Model(vaoID, nbind);
}

Texture			*Loader::loadTexture(std::string path) {
	Texture		texture = Texture(path);

	textures.push_front(texture.getTextureID());
	return new Texture(texture);
}