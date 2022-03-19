#pragma once

#include "ComponentCommon.h"

#include <CherrySoda/Input/MInput.h>
#include <CherrySoda/Util/Camera.h>

#include "Logging.h"



namespace cherrysoda
{
	/*
	* TODO:
	* 
	* Create Manager class for different controller implementations.
	* And that one should be updated on each tick and he decides which
	* actual controller implementation to use...
	* 
	* Thus we can:
	*	1) Switch dynamic to different controller implementations, and e.g. react to connected input device
	*	2) Fully control the behavior in a multiplatform environment
	*/


	class CameraController : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(CameraController, Component);

		CameraController(Camera* camera) : CameraController(true, false)
		{
			m_camera = camera;
		}

		void Update();

	private:
		Camera* m_camera = nullptr;

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		CameraController(bool active, bool visible) : base(active, visible) {};
	};
}