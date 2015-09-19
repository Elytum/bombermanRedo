#include <maths/Matrix.hpp>
#include <iostream>
#include <cstring>

// Set a matrix with it's rotation of "angle"

static void		get_rotate_floats(float f[16], Vec3 axis, float angle)
{
	float			c;
	float			s;
	float			cm;
	float			xyz[6];

	c = cos(angle);
	s = sin(angle);
	cm = 1.0f - c;
	xyz[0] = axis.x * axis.y;
	xyz[1] = axis.y * axis.z;
	xyz[2] = axis.x * axis.z;
	xyz[3] = axis.x * s;
	xyz[4] = axis.y * s;
	xyz[5] = axis.z * s;
	f[0] = axis.x * axis.x * cm + c;
	f[4] = xyz[0] * cm + xyz[5];
	f[8] = xyz[2] * cm - xyz[4];
	f[1] = xyz[0] * cm - xyz[5];
	f[5] = axis.y * axis.y * cm + c;
	f[9] = xyz[1] * cm + xyz[3];
	f[2] = xyz[2] * cm + xyz[4];
	f[6] = xyz[1] * cm - xyz[3];
	f[10] = axis.z * axis.z * cm + c;
}

// Matrix::Matrix(void)	{
// }

// Matrix::Matrix(GLuint vaoID, GLsizei vertexCount) :	vaoID(vaoID), vertexCount(vertexCount)	{
// }

// Matrix::Matrix( Matrix const & cpy)	{
// 	*this = cpy;
// }

// Matrix::~Matrix() {

// }

// Matrix&		Matrix::operator=(Matrix const & cpy)	{
// 	vaoID = cpy.vaoID;
// 	vertexCount = cpy.vertexCount;
// 	return *this;
// }

// GLuint		Matrix::getVaoID(void)	{
// 	return (vaoID);
// }

// GLsizei		Matrix::getVertexCount(void)	{
// 	return (vertexCount);
// }


/*
** Set a matrix to it's identity state
*/

Matrix		Matrix::identity(void) {
	return Matrix(	1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);

}

Matrix::Matrix(	float a, float b, float c, float d,
				float e, float f, float g, float h,
				float i, float j, float k, float l,
				float m, float n, float o, float p)	{
	data[0] = a;
	data[1] = b;
	data[2] = c;
	data[3] = d;
	data[4] = e;
	data[5] = f;
	data[6] = g;
	data[7] = h;
	data[8] = i;
	data[9] = j;
	data[10] = k;
	data[11] = l;
	data[12] = m;
	data[13] = n;
	data[14] = o;
	data[15] = p;
}

Matrix::Matrix(	float tab[16] )	{
	std::memcpy(data, tab, sizeof(data));
}

Matrix::Matrix(void) {
	*this = Matrix::identity();
}

Matrix::~Matrix(void) {
}

Matrix::Matrix(const Matrix & cpy) {
	*this = cpy;
}

/*
**								TOOLS
*/

float				*Matrix::getDataLocation(void)	{
	return data;
}

// Set a matrix according to it's translation of x, y and z


Matrix				Matrix::translate(float x, float y, float z)
{
	data[3] += data[0] * x + data[1] * y + data[2] * z;
	data[7] += data[4] * x + data[5] * y + data[6] * z;
	data[11] += data[8] * x + data[9] * y + data[10] * z;
	data[15] += data[12] * x + data[12] * y + data[13] * z;
	return *this;
}

// Set a matrix according to it's rotation of "angle" on the axis "axis"

Matrix				Matrix::rotate(Vec3 axis, float angle)
{
							float			f[16];
							float			tmp[16];

							axis.normalize();
							get_rotate_floats(f, axis, angle);
							tmp[0] = data[0] * f[0] + data[1] * f[4] + data[2] * f[8];
							tmp[4] = data[4] * f[0] + data[5] * f[4] + data[6] * f[8];
							tmp[8] = data[8] * f[0] + data[9] * f[4] + data[10] * f[8];
							tmp[12] = data[12] * f[0] + data[13] * f[4] + data[14] * f[8];
							tmp[1] = data[0] * f[1] + data[1] * f[5] + data[2] * f[9];
							tmp[5] = data[4] * f[1] + data[5] * f[5] + data[6] * f[9];
							tmp[9] = data[8] * f[1] + data[9] * f[5] + data[10] * f[9];
							tmp[13] = data[12] * f[1] + data[13] * f[5] + data[14] * f[9];
							data[2] = data[0] * f[2] + data[1] * f[6] + data[2] * f[10];
							data[6] = data[4] * f[2] + data[5] * f[6] + data[6] * f[10];
							data[10] = data[8] * f[2] + data[9] * f[6] + data[10] * f[10];
							data[14] = data[12] * f[2] + data[13] * f[6] + data[12] * f[10];
							data[0] = tmp[0];
							data[4] = tmp[4];
							data[8] = tmp[8];
							data[12] = tmp[12];
							data[1] = tmp[1];
							data[5] = tmp[5];
							data[9] = tmp[9];
							data[13] = tmp[13];
	return *this;
}

Matrix				Matrix::combine(Matrix m)
{
	float			*s = m.getDataLocation();
	static float	t[16];

	t[0] = data[0] * s[0] + data[4] * s[1] + data[8] * s[2] + data[12] * s[3];
	t[1] = data[1] * s[0] + data[5] * s[1] + data[9] * s[2] + data[13] * s[3];
	t[2] = data[2] * s[0] + data[6] * s[1] + data[10] * s[2] + data[14] * s[3];
	t[3] = data[3] * s[0] + data[7] * s[1] + data[11] * s[2] + data[15] * s[3];
	t[4] = data[0] * s[4] + data[4] * s[5] + data[8] * s[6] + data[12] * s[7];
	t[5] = data[1] * s[4] + data[5] * s[5] + data[9] * s[6] + data[13] * s[7];
	t[6] = data[2] * s[4] + data[6] * s[5] + data[10] * s[6] + data[14] * s[7];
	t[7] = data[3] * s[4] + data[7] * s[5] + data[11] * s[6] + data[15] * s[7];
	t[8] = data[0] * s[8] + data[4] * s[9] + data[8] * s[10] + data[12] * s[11];
	t[9] = data[1] * s[8] + data[5] * s[9] + data[9] * s[10] + data[13] * s[11];
	t[10] = data[2] * s[8] + data[6] * s[9] + data[10] * s[10] + data[14] * s[11];
	t[11] = data[3] * s[8] + data[7] * s[9] + data[11] * s[10] + data[15] * s[11];
	t[12] = data[0] * s[12] + data[4] * s[13] + data[8] * s[14] + data[12] * s[15];
	t[13] = data[1] * s[12] + data[5] * s[13] + data[9] * s[14] + data[13] * s[15];
	t[14] = data[2] * s[12] + data[6] * s[13] + data[10] * s[14] + data[14] * s[15];
	t[15] = data[3] * s[12] + data[7] * s[13] + data[11] * s[14] + data[15] * s[15];
	*this = Matrix(t);
	// matrix_copy(data, (float*)t);
	return *this;
}

// Set a matrix according to it's scaling of x, y and z

Matrix				Matrix::scale(float x, float y, float z)
{
	data[0] = data[0] * x;
	data[4] = data[4] * x;
	data[8] = data[8] * x;
	data[12] = data[12] * x;
	data[1] = data[1] * y;
	data[5] = data[5] * y;
	data[9] = data[9] * y;
	data[13] = data[13] * y;
	data[2] = data[2] * z;
	data[6] = data[6] * z;
	data[10] = data[10] * z;
	data[14] = data[14] * z;
	return *this;
}

Matrix&		Matrix::operator=(Matrix const & cpy)	{
	std::memcpy (data, cpy.data, sizeof(data));
	return *this;
}

Matrix		Matrix::createTransformationMatrix(float tra[3], float rot[3], float sca[3])	{
	Matrix		m;

	m.translate(tra[0], tra[1], tra[2]);
	m.rotate(Vec3(1, 0, 0), rot[0]);
	m.rotate(Vec3(0, 1, 0), rot[1]);
	m.rotate(Vec3(0, 0, 1), rot[2]);
	m.scale(sca[0], sca[1], sca[2]);
	return (m);
}
/*
	t_point3	rev;

	matrix_identity(matrix);
	matrix_rotate(matrix, new_point3(1, 0, 0), camera->pitch);
	matrix_rotate(matrix, new_point3(0, 1, 0), camera->yaw);
	matrix_rotate(matrix, new_point3(0, 0, 1), camera->roll);
	rev = new_point3(camera->pos[0], camera->pos[1], camera->pos[2]);
	rev = vec_rev_sign(rev);
	matrix_translate(matrix, rev.x, rev.y, rev.z);
*/
Matrix		Matrix::createViewMatrix(Camera camera)	{
	Matrix		m;
	float		*cameraPos = camera.getPosition();

	m.rotate(Vec3(1, 0, 0), (float) TO_RADIAN(camera.getPitch()));
	m.rotate(Vec3(1, 0, 0), (float) TO_RADIAN(camera.getYaw()));
	m.rotate(Vec3(1, 0, 0), (float) TO_RADIAN(camera.getRoll()));

	m.translate(-cameraPos[0], -cameraPos[1], -cameraPos[2]);
	return (m);
}


/*
** Set a matrix to it's identity state
*/

// void				matrix_identity(float *m)
// {
// 	m[0] = 1;
// 	m[1] = 0;
// 	m[2] = 0;
// 	m[3] = 0;
// 	m[4] = 0;
// 	m[5] = 1;
// 	m[6] = 0;
// 	m[7] = 0;
// 	m[8] = 0;
// 	m[9] = 0;
// 	m[10] = 1;
// 	m[11] = 0;
// 	m[12] = 0;
// 	m[13] = 0;
// 	m[14] = 0;
// 	m[15] = 1;
// }

// /*
// ** Set a matrix with it's rotation of "angle"
// */

// void			get_rotate_floats(float f[16], Vec3 axis, float angle)
// {
// 	float			c;
// 	float			s;
// 	float			cm;
// 	float			xyz[6];

// 	c = cos(angle);
// 	s = sin(angle);
// 	cm = 1.0f - c;
// 	xyz[0] = axis.x * axis.y;
// 	xyz[1] = axis.y * axis.z;
// 	xyz[2] = axis.x * axis.z;
// 	xyz[3] = axis.x * s;
// 	xyz[4] = axis.y * s;
// 	xyz[5] = axis.z * s;
// 	f[0] = axis.x * axis.x * cm + c;
// 	f[4] = xyz[0] * cm + xyz[5];
// 	f[8] = xyz[2] * cm - xyz[4];
// 	f[1] = xyz[0] * cm - xyz[5];
// 	f[5] = axis.y * axis.y * cm + c;
// 	f[9] = xyz[1] * cm + xyz[3];
// 	f[2] = xyz[2] * cm + xyz[4];
// 	f[6] = xyz[1] * cm - xyz[3];
// 	f[10] = axis.z * axis.z * cm + c;
// }

/*
** Set "dst" according to "src"
*/

// void				matrix_copy(float *dst, float *src)
// {
// 	dst[0] = src[0];
// 	dst[1] = src[1];
// 	dst[2] = src[2];
// 	dst[3] = src[3];
// 	dst[4] = src[4];
// 	dst[5] = src[5];
// 	dst[6] = src[6];
// 	dst[7] = src[7];
// 	dst[8] = src[8];
// 	dst[9] = src[9];
// 	dst[10] = src[10];
// 	dst[11] = src[11];
// 	dst[12] = src[12];
// 	dst[13] = src[13];
// 	dst[14] = src[14];
// 	dst[15] = src[15];
// }

/*
** Set "dst" according to "dst" * "s"
*/

// float				*matrix_combine(float *dst, float *s)
// {
// 	static float	t[16];

// 	t[0] = dst[0] * s[0] + dst[4] * s[1] + dst[8] * s[2] + dst[12] * s[3];
// 	t[1] = dst[1] * s[0] + dst[5] * s[1] + dst[9] * s[2] + dst[13] * s[3];
// 	t[2] = dst[2] * s[0] + dst[6] * s[1] + dst[10] * s[2] + dst[14] * s[3];
// 	t[3] = dst[3] * s[0] + dst[7] * s[1] + dst[11] * s[2] + dst[15] * s[3];
// 	t[4] = dst[0] * s[4] + dst[4] * s[5] + dst[8] * s[6] + dst[12] * s[7];
// 	t[5] = dst[1] * s[4] + dst[5] * s[5] + dst[9] * s[6] + dst[13] * s[7];
// 	t[6] = dst[2] * s[4] + dst[6] * s[5] + dst[10] * s[6] + dst[14] * s[7];
// 	t[7] = dst[3] * s[4] + dst[7] * s[5] + dst[11] * s[6] + dst[15] * s[7];
// 	t[8] = dst[0] * s[8] + dst[4] * s[9] + dst[8] * s[10] + dst[12] * s[11];
// 	t[9] = dst[1] * s[8] + dst[5] * s[9] + dst[9] * s[10] + dst[13] * s[11];
// 	t[10] = dst[2] * s[8] + dst[6] * s[9] + dst[10] * s[10] + dst[14] * s[11];
// 	t[11] = dst[3] * s[8] + dst[7] * s[9] + dst[11] * s[10] + dst[15] * s[11];
// 	t[12] = dst[0] * s[12] + dst[4] * s[13] + dst[8] * s[14] + dst[12] * s[15];
// 	t[13] = dst[1] * s[12] + dst[5] * s[13] + dst[9] * s[14] + dst[13] * s[15];
// 	t[14] = dst[2] * s[12] + dst[6] * s[13] + dst[10] * s[14] + dst[14] * s[15];
// 	t[15] = dst[3] * s[12] + dst[7] * s[13] + dst[11] * s[14] + dst[15] * s[15];
// 	matrix_copy(dst, (float*)t);
// 	return (dst);
// }

// /*
// ** Set a matrix according to it's translation of x, y and z
// */

// void				matrix_translate(float *m, float x, float y, float z)
// {
// 	m[3] += m[0] * x + m[1] * y + m[2] * z;
// 	m[7] += m[4] * x + m[5] * y + m[6] * z;
// 	m[11] += m[8] * x + m[9] * y + m[10] * z;
// 	m[15] += m[12] * x + m[12] * y + m[13] * z;
// }

/*
** Set a matrix according to it's rotation of "angle" on the axis "axis"
*/

// void				matrix_rotate(float *m, Vec3 axis, float angle)
// {
// 	float			f[16];
// 	float			tmp[16];

// 	axis = vec_normalize(axis);
// 	get_rotate_floats(f, axis, angle);
// 	tmp[0] = m[0] * f[0] + m[1] * f[4] + m[2] * f[8];
// 	tmp[4] = m[4] * f[0] + m[5] * f[4] + m[6] * f[8];
// 	tmp[8] = m[8] * f[0] + m[9] * f[4] + m[10] * f[8];
// 	tmp[12] = m[12] * f[0] + m[13] * f[4] + m[14] * f[8];
// 	tmp[1] = m[0] * f[1] + m[1] * f[5] + m[2] * f[9];
// 	tmp[5] = m[4] * f[1] + m[5] * f[5] + m[6] * f[9];
// 	tmp[9] = m[8] * f[1] + m[9] * f[5] + m[10] * f[9];
// 	tmp[13] = m[12] * f[1] + m[13] * f[5] + m[14] * f[9];
// 	m[2] = m[0] * f[2] + m[1] * f[6] + m[2] * f[10];
// 	m[6] = m[4] * f[2] + m[5] * f[6] + m[6] * f[10];
// 	m[10] = m[8] * f[2] + m[9] * f[6] + m[10] * f[10];
// 	m[14] = m[12] * f[2] + m[13] * f[6] + m[12] * f[10];
// 	m[0] = tmp[0];
// 	m[4] = tmp[4];
// 	m[8] = tmp[8];
// 	m[12] = tmp[12];
// 	m[1] = tmp[1];
// 	m[5] = tmp[5];
// 	m[9] = tmp[9];
// 	m[13] = tmp[13];
// }

// /*
// ** Set a matrix according to it's scaling of x, y and z
// */

// void				matrix_scale(float *m, float x, float y, float z)
// {
// 	m[0] = m[0] * x;
// 	m[4] = m[4] * x;
// 	m[8] = m[8] * x;
// 	m[12] = m[12] * x;
// 	m[1] = m[1] * y;
// 	m[5] = m[5] * y;
// 	m[9] = m[9] * y;
// 	m[13] = m[13] * y;
// 	m[2] = m[2] * z;
// 	m[6] = m[6] * z;
// 	m[10] = m[10] * z;
// 	m[14] = m[14] * z;
// }
