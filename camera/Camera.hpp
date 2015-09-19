#ifndef CAMERA_HPP
# define CAMERA_HPP

class Camera
{
	public:

		Camera( void );
		Camera( Camera const & cpy);
		~Camera( void );
		float			*getPosition(void);
		float			getPitch(void);
		float			getRoll(void);
		float			getYaw(void);
		void			move(int key);
		Camera&			operator=(Camera const & cpy);

	private:
		float			pitch;
		float			roll;
		float			yaw;
		float			position[3];
};

#endif
