#ifndef MATRIX_HPP
# define MATRIX_HPP

#define TO_RADIAN(x) (x / 180.0f * M_PI)
#include <maths/Vec3.hpp>
#include <cmath>
#include <Camera/Camera.hpp>

class Matrix
{
	public:
		static Matrix	identity(void);
		static Matrix	createTransformationMatrix(float tra[3], float rot[3], float sca[3]);
		static Matrix	createViewMatrix(Camera camera);
		Matrix(void);
		Matrix(	float a, float b, float c, float d,
				float e, float f, float g, float h,
				float i, float j, float k, float l,
				float m, float n, float o, float p);
		Matrix(float tab[16]);
		~Matrix(void);
		Matrix(const Matrix & cpy);
		Matrix			matrix_rotate(Vec3 axis, float angle);
		Matrix			matrix_combine(Matrix s);

		// private:
		// 	GLuint		vaoID;
		// 	GLsizei		vertexCount;
		float			*getDataLocation(void);
		// Matrix			get_rotate_floats(Vec3 axis, float angle);
		Matrix			copy(Matrix m);
		Matrix			combine(Matrix m);
		Matrix			translate(float x, float y, float z);
		Matrix			rotate(Vec3 axis, float angle);
		Matrix			scale(float x, float y, float z);
		Matrix&			operator=(Matrix const & cpy);
	private:
		float			data[16];
};

#endif
