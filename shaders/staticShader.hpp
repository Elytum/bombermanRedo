#ifndef STATICSHADER_HPP
# define STATICSHADER_HPP

#include <Shaders/Shader.hpp>
#include <Camera/Camera.hpp>

class StaticShader : public Shader
{
	public:
		StaticShader(void);
		StaticShader&			operator=(StaticShader const & cpy);
		void					loadTransformationMatrix(float matrix[16]);
		void					loadProjectionMatrix(float matrix[16]);
		void					loadViewMatrix(Camera camera);

	private:
		const std::string		vertexFile;
		const std::string		fragmentFile;
		GLuint					locationTransformationMatrix;
		GLuint					locationViewMatrix;
		GLuint					locationProjectionMatrix;

	protected:
		virtual void			getAllUniformLocations(void);
		virtual void			bindAttributes(void);
};

#endif
