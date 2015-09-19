#ifndef SHADER_HPP
# define SHADER_HPP

#include <graphic.h>
#include <iostream>

class Shader
{

	public:
		Shader(void);
		Shader(std::string vertexFile, std::string fragmentFile);
		~Shader();
		Shader( Shader const & cpy);
		Shader&		operator=(Shader const & cpy);
		GLuint			loadShaders(const std::string vertexFile, const std::string fragmentFile);
		void			start(void);
		void			stop(void);
		void			cleanUp(void);

	private:
		GLuint		programID;
		GLuint		vertexShaderID;
		GLsizei		fragmentShaderID;

	protected:
		int				getUniformLocation(std::string uniformName);
		void			bindAttribute(GLuint attribute, std::string variableName);
		void			beginCreation(std::string vertexFile, std::string fragmentFile);
		void			endCreation(void);
		virtual void	getAllUniformLocations(void);
		virtual void	bindAttributes(void);
		void			loadFloat(int location, float value);
		void			loadVector(int location, float *vector);
		void			loadBoolean(int location, bool value);
		void			loadMatrix(int location, float *matrix);
};

#endif
