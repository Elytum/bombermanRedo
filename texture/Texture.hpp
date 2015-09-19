#ifndef Texture_HPP
# define Texture_HPP

#include <graphic.h>
// #include <Soil.h>
#include <iostream>

class Texture
{

	public:
		Texture(void);
		Texture(std::string name);
		~Texture();
		Texture( Texture const & cpy);
		Texture&		operator=(Texture const & cpy);
		GLuint     		getTextureID(void);
	private:
		std::string		name;
		unsigned char	*image;
		int				width;
		int				height;
		GLuint			textureID;
};

#endif
