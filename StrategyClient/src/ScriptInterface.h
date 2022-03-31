#pragma once

#include "ComponentCommon.h"
#include "Logging.h"

// TESTING FEATURE
// SCRIPTS REACTING AND GENERATING IN GAME EVENTS
#include "EventSystem.h"


namespace cherrysoda
{
	class Lua;

	/*
	* Functions every script should have (At least the bodies).
	* Main - for main entity udpate in scene.
	* Begin - for when the entity is added to scene.
	* End - for when the entity is removed from scene.
	*/
	class ScriptInterface : public Component
	{
		friend class Lua;
	protected:
		CHERRYSODA_DECLARE_COMPONENT(ScriptInterface, Component);

		ScriptInterface() : ScriptInterface(true, false)
		{
		}

		void EntityAdded(Scene* scene) override
		{
			OnEntityCreate();
		}
		void EntityRemoved(Scene* scene) override
		{
			OnEntityDestroy();
		}
		void Update() override
		{
			OnEntityUpdate();
		}

		// DEFAULT LOAD AND UNLOAD
		virtual bool Initialize(const String&) = 0;
		virtual void Terminate() = 0;

		// MAIN FUNCTIONS
		// When the entity we are attached to is instantiated into scene,
		// this function will be called.
		virtual void OnEntityCreate() = 0;
		// When the entity we are attached to is unloaded from the scene,
		// this function will be called.
		virtual void OnEntityDestroy() = 0;
		// Function named Tick and not Update as to highlight that its not
		// intended to be executed every frame, but rather more sparsely.
		// Function calls the Main function of the script.
		virtual bool OnEntityTick(float) = 0;
		// Function called on every frame.
		// Should be used sparsely and have small functionality in order to safe FPS.
		// Could be used for example by GUI.
		virtual void OnEntityUpdate() = 0;

		virtual bool Execute(void) = 0;

		// REACT TO EVENT
		// Function called by EventSystem if the event we are listening to has happened.
		virtual void OnEvent(Event*) = 0;


		// UTIL
		bool Loaded() { return m_loaded; }
		void Loaded(bool v) { m_loaded = v; }
		String Error() { return m_error; }
		void Error(String e) { m_error = e; }


	protected:
		bool m_loaded;
		String m_error;
		String m_filepath;


	protected:
		
	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		ScriptInterface(bool active, bool visible) : base(active, visible) {};
	};
}