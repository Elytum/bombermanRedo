#include <Shaders/StaticShader.hpp>
#include <Maths/Matrix.hpp>

StaticShader::StaticShader(void) {
	beginCreation(	"shaders/SimpleVertexShader.vertexshader",
					"shaders/SimpleFragmentShader.fragmentshader");
	bindAttributes();
	endCreation();
	getAllUniformLocations();
}

void	StaticShader::bindAttributes(void)	{
	bindAttribute(0, "position");
	bindAttribute(1, "texturecoords");
}

void	StaticShader::getAllUniformLocations(void)	{
	locationTransformationMatrix = getUniformLocation("transformationMatrix");
	locationViewMatrix = getUniformLocation("viewMatrix");
	locationProjectionMatrix = getUniformLocation("projectionMatrix");
}

void	StaticShader::loadTransformationMatrix(float transformation[16])	{
	// dprintf(1, "Loading:\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n",
	// 						transformation[0], transformation[1], transformation[2], transformation[3],
	// 						transformation[4], transformation[5], transformation[6], transformation[7],
	// 						transformation[8], transformation[9], transformation[10], transformation[11],
	// 						transformation[12], transformation[13], transformation[14], transformation[15]);
	loadMatrix(locationTransformationMatrix, transformation);
}

void	StaticShader::loadViewMatrix(Camera camera)	{
	// dprintf(1, "Loading:\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n",
	// 						projection[0], projection[1], projection[2], projection[3],
	// 						projection[4], projection[5], projection[6], projection[7],
	// 						projection[8], projection[9], projection[10], projection[11],
	// 						projection[12], projection[13], projection[14], projection[15]);
	loadMatrix(locationViewMatrix, Matrix::createViewMatrix(camera).getDataLocation());
}

void	StaticShader::loadProjectionMatrix(float projection[16])	{
	// dprintf(1, "Loading:\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n\t%f  %f  %f  %f\n",
	// 						projection[0], projection[1], projection[2], projection[3],
	// 						projection[4], projection[5], projection[6], projection[7],
	// 						projection[8], projection[9], projection[10], projection[11],
	// 						projection[12], projection[13], projection[14], projection[15]);
	loadMatrix(locationProjectionMatrix, projection);
}

StaticShader&		StaticShader::operator=(StaticShader const & cpy)	{
	return *this;
}