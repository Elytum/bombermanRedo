// #ifndef MATH_HPP
// # define MATH_HPP
// # include <math.h>

// # define TO_RADIAN(x) (x / 180.0f * M_PI)

// typedef struct	s_vec3
// {
// 	float	x;
// 	float	y;
// 	float	z;
// }				t_vec3;

// t_vec3			new_vec3(float x, float y, float z);
// static double	vec_norm(t_vec3 vec);
// t_vec3			vec_normalize(t_vec3 vec);
// t_vec3			vec_rev_sign(t_vec3 vec);

// #endif

#ifndef VEC3_HPP
# define VEC3_HPP

// #include <graphic.h>

class Vec3
{
	public:
		Vec3(float x, float y, float z);
		~Vec3(void);
		Vec3(const Vec3 & cpy);
		float			x;
		float			y;
		float			z;
		double			norm(void);
		Vec3			normalize(void);
		Vec3&			operator=(Vec3 const & cpy);

	// private:
};

#endif
