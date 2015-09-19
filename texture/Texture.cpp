#include <Texture/Texture.hpp>

static GLuint	load_texture(unsigned char *data, int width, int height)
{
    GLuint              textureID = 0;

    // glGenTextures(1, &textureID);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, textureID);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4);
    // SOIL_free_image_data(data);
    // if (textureID == -1) {
    //     return -1;
    // }
    return (textureID);
}

Texture::Texture(void)  {
}

Texture::Texture(std::string name) :   name(name)  {
    // image = SOIL_load_image(name.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    // textureID = load_texture(image, width, height);
}

Texture::Texture( Texture const & cpy)    {
    *this = cpy;
}

Texture::~Texture() {
}

GLuint      Texture::getTextureID(void) {
    return textureID;
}

Texture&      Texture::operator=(Texture const & cpy) {
    name = cpy.name;
    image = cpy.image;
    width = cpy.width;
    height = cpy.height;
    textureID = cpy.textureID;
    return *this;
}
