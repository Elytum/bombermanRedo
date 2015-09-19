#include <Camera/Camera.hpp>
#include <graphic.h>
#include <string.h>

Camera::Camera() : pitch(0), roll(0), yaw(0)	{
	memset(position, 0, sizeof(position));
}

Camera::~Camera() {
}

Camera::Camera( Camera const & cpy)	{
	*this = cpy;
}


void			Camera::move(int key)	{
	switch (key) {
		case GLFW_KEY_W:
			position[2] -= .2;
			break;
		case GLFW_KEY_S:
			position[2] += .2;
			break;
		case GLFW_KEY_A:
			position[0] -= .2;
			break;
		case GLFW_KEY_D:
			position[0] += .2;
			break;
	}
}

float			*Camera::getPosition(void)	{
	return position;
}

float			Camera::getPitch(void)	{
	return pitch;
}

float			Camera::getYaw(void)	{
	return yaw;
}


float			Camera::getRoll(void)	{
	return roll;
}

Camera&			Camera::operator=(Camera const & cpy)	{
	memcpy(position, cpy.position, sizeof(position));
	pitch = cpy.pitch;
	roll = cpy.roll;
	yaw = cpy.yaw;
	return *this;
}
