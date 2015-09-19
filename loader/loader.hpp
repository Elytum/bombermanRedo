#ifndef LOADER_HPP
# define LOADER_HPP

#include <graphic.h>
#include <Model/Model.hpp>
#include <Texture/Texture.hpp>
#include <list>

class Loader
{

	public:
		Loader(void);
		~Loader();
		Loader( Loader const & cpy);
		Loader&			operator=(Loader const & cpy);

		Model			*loadToVAO(	float positions[], GLsizeiptr nbpos,
									float texture[], GLsizeiptr nbtex,
									GLuint indices[], GLsizeiptr nbind);
		Texture			*loadTexture(std::string path);
		void			cleanUp(void);

	private:
		std::list<GLuint>	vaos;
		std::list<GLuint>	vbos;
		std::list<GLuint>	textures;

		GLuint		createVAO(void);
		void		unbindVAO(void);
		GLuint		storeDataInAttributeList(int attribute,
						int coordsize, GLvoid *data, GLsizeiptr len);
		GLuint		bindIndicesBuffer(GLuint *indices, GLsizeiptr len);
};

#endif
