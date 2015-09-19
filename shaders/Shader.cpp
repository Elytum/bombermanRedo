#include <graphic.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <Shaders/Shader.hpp>
#include <sstream>

/*
**	Load a shader into opengl using a "vertex file" and a "fragment file"
*/

static GLuint		loadShader(std::string filename, GLuint type)
{
		// Create the shaders
	GLuint shaderID = glCreateShader(type);

	// Read the Shader code from the file
	std::string shaderCode;
	std::ifstream shaderStream(filename, std::ios::in);
	if(shaderStream.is_open())
	{
		std::string Line = "";
		while(getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}

	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile Shader
	std::cout << "Compiling shader : " + filename << std::endl;
	char const * SourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &SourcePointer , NULL);
	glCompileShader(shaderID);

	// Check  Shader
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> ShaderErrorMessage(infoLogLength);
	glGetShaderInfoLog(shaderID, infoLogLength, NULL, &ShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &ShaderErrorMessage[0]);
	return (shaderID);
}

Shader::Shader(void)	{
}

void		Shader::beginCreation(std::string vertexFile, std::string fragmentFile) {
	vertexShaderID = loadShader(vertexFile, GL_VERTEX_SHADER);
	fragmentShaderID = loadShader(fragmentFile, GL_FRAGMENT_SHADER);
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
}

void		Shader::endCreation(void) {
	glLinkProgram(programID);
	glValidateProgram(programID);
}

Shader::Shader(std::string vertexFile, std::string fragmentFile)	{
	beginCreation(vertexFile, fragmentFile);
	bindAttributes();
	endCreation();
	getAllUniformLocations();
}

Shader::Shader( Shader const & cpy)	{
	*this = cpy;
}

Shader::~Shader() {

}

Shader&		Shader::operator=(Shader const & cpy)	{
	programID = cpy.programID;
	vertexShaderID = cpy.vertexShaderID;
	fragmentShaderID = cpy.fragmentShaderID;
	return *this;
}

void	Shader::start(void)	{
	glUseProgram(programID);
}

void	Shader::stop(void)	{
	glUseProgram(0);
}

void	Shader::cleanUp(void)	{
	stop();
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteProgram(programID);
}

void	Shader::bindAttribute(GLuint attribute, std::string variableName)	{
	glBindAttribLocation(programID, attribute, variableName.c_str());
}

void	Shader::bindAttributes(void) {
}

void	Shader::getAllUniformLocations(void)	{
}

int		Shader::getUniformLocation(std::string uniformName)	{
	return (glGetUniformLocation(programID, uniformName.c_str()));
}

void	Shader::loadFloat(int location, float value)	{
	glUniform1f(location, value);
}

void	Shader::loadVector(int location, float *vector)	{
	glUniform3f(location, vector[0], vector[1], vector[2]);
}

void	Shader::loadBoolean(int location, bool value)	{
	glUniform1f(location, (value == 0) ? 0 : 1);
}

void	Shader::loadMatrix(int location, float *matrix)	{
	glUniformMatrix4fv(location, 1, GL_TRUE, matrix);
}