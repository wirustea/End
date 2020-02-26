#ifndef _PARAMS_H
#define _PARAMS_H

namespace PARAMS
{
	static float st_factor = 1.0;     //platform related factor
	static float rotation_factor = 1.25f;
	static float wheel_rotation_factor = 0.8;
	static float body_swing_factor = 2.0;
	static float m_ACCFRICTION = 5.5f;// friction
	static float m_ACCAIR = 1.0f;     //acceleration in air
	static float m_ACCLAND = 10.0f;   //acceleration on land
	static float m_ACCG = 60.0f;       //acceletation of gravity
	static float m_dt = 0.01f;
	static float m_MaxV = 50.0f;
	static float m_MinV = -20.0f;
	static float first_person_camera_pitch_factor = 1.0f;
	static float third_person_camera_pitch_factor = 1.0f;
	static float camera_auto_move_dely = 1.0f;
}

#endif