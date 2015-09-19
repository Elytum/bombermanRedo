#ifndef RENDER_HPP
# define RENDER_HPP

void display(void);
void setanim(int i);
void perspective(float fov, float aspect, float znear, float zfar);

// # ifndef CAMERA
// #  define CAMERA
// #define DIMETRIC 30		// 2:1 'isometric' as seen in pixel art
// 	struct {
// 		float distance;
// 		float yaw;
// 		float pitch;
// 		float center[3];
// 	}	camera = { 3, 45, -DIMETRIC, { 0, 1, 0 } };
// # endif

#endif
