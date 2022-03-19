#include "CameraController.h"

namespace cherrysoda
{
	void CameraController::Update()
	{
		float speed_modifier = 1.0f;

		if (MInput::Keyboard()->Check(Keys::LeftShift))
		{
			speed_modifier = 5.0f;
			LOG_GAME_INFO("Shift");
		}
		if (MInput::Keyboard()->Released(Keys::OemTilde))
		{
			Logger::ToggleGamelog();
		}
		if (MInput::Keyboard()->Check(Keys::A))
		{
			m_camera->MovePositionX(-1.0f * speed_modifier);
			LOG_GAME_INFO("Move Left");
		}
		if (MInput::Keyboard()->Check(Keys::W))
		{
			m_camera->MovePositionY(1.0f * speed_modifier);
			LOG_GAME_INFO("Move Up");
		}
		if (MInput::Keyboard()->Check(Keys::D))
		{
			m_camera->MovePositionX(1.0f * speed_modifier);
			LOG_GAME_INFO("Move Right");
		}
		if (MInput::Keyboard()->Check(Keys::S))
		{
			m_camera->MovePositionY(-1.0f * speed_modifier);
			LOG_GAME_INFO("Move Down");
		}

		m_camera->UpdateMatrices();
	}




}