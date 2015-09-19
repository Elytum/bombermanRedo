#include <Maths/Vec3.hpp>
#include <cmath>

// /*
// ** Returns a vector according to x, y and z values
// */

// Vec3		new_vec3(float x, float y, float z)
// {
// 	Vec3	p;

// 	p.x = x;
// 	p.y = y;
// 	p.z = z;
// 	return (p);
// }

// /*
// ** Returns the reverse of the vector
// */

// Vec3		vec_rev_sign(Vec3 vec)
// {
// 	return (new_vec3(-vec.x, -vec.y, -vec.z));
// }

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z)	{
}

Vec3::Vec3( Vec3 const & cpy)	{
	*this = cpy;
}

Vec3::~Vec3() {

}

Vec3&		Vec3::operator=(Vec3 const & cpy)	{
	x = cpy.x;
	y = cpy.y;
	z = cpy.z;
	return *this;
}

// Returns the norm of a vector

double			Vec3::norm(void)	{
	return (sqrt(x * x + y * y + z * z));
}


// Returns the vector normalized

Vec3			Vec3::normalize(void)	{
	double		n;

	n = norm();
	if (n == 0)
		n = 0.0001f;
	x = x / n;
	y = y / n;
	z = z / n;
	return *this;
}