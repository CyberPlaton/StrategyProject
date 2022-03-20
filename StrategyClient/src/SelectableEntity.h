#pragma once

#include "ComponentCommon.h"

#include <CherrySoda/Components/CollidableComponent.h>
#include <CherrySoda/Components/Graphics/Sprite.h>
#include <CherrySoda/Colliders/Hitbox.h>
#include <CherrySoda/Colliders/Circle.h>
#include <CherrySoda/Components/Graphics/Sprite.h>
#include <CherrySoda/Input/MInput.h>
#include <CherrySoda/Entity.h>
#include <CherrySoda/Scene.h>
#include <CherrySoda/Util/Camera.h>
#include <CherrySoda/Renderers/Renderer.h>

#include "Timer.h"
#include "Logging.h"


namespace cherrysoda
{
	/*
	* Note:
	* It is required that the Entity is visible and collidable.
	* This means it needs a "Sprite" and "CollidableComponent" components added.
	*/
	class SelectableEntity : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(SelectableEntity, Component);
		
		SelectableEntity(int seconds_hovering_time = 0) : SelectableEntity(true, true)
		{
			m_hoverTimer = seconds_hovering_time;
		}

		void Update() override;

		virtual void OnHeldLMB()     {}
		virtual void OnHeldRMB()     {}
		virtual void OnPressedLMB()  {}
		virtual void OnPressedRMB()  {}
		virtual void OnReleasedLMB() {}
		virtual void OnReleasedRMB() {}

		virtual void OnHoverEnter()	 {}
		virtual void OnHoverExit()	 {}

	private:

		Timer m_timer;
		bool m_timerStarted = false;
		int m_hoverTimer;

	private:
		SelectableEntity(bool active, bool visible) : Component(active, visible)
		{
		}

	};
}