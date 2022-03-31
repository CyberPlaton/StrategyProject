#pragma once

#include "ScriptInterface.h"
#include "Lua.h"

namespace cherrysoda
{
	class ScriptComponent : public ScriptInterface
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(ScriptComponent, ScriptInterface);

		ScriptComponent(const String& filepath) : m_luaState(nullptr)
		{
			if (!Initialize(filepath))
			{
				Terminate();
				RemoveSelf();
				LOG_DBG_ERROR("[{:.4f}] Script not initialized! Error: {}", APP_RUN_TIME(), filepath.c_str(), m_error.c_str());
				LOG_GAME_ERROR("[%.4f] Script not initialized! Error: %s", APP_RUN_TIME(), filepath.c_str(), m_error.c_str());
			}

			LOG_DBG_INFO("[{:.4f}] Script {} initialized!", APP_RUN_TIME(), filepath.c_str());
			LOG_GAME_INFO("[%.4f] Script %s initialized!", APP_RUN_TIME(), filepath.c_str());
		}

		bool Initialize(const String& filepath) override;
		void Terminate() override;
		virtual void OnEntityCreate() override;
		virtual void OnEntityDestroy() override;
		virtual bool OnEntityTick(float dt)override;
		virtual void OnEntityUpdate() override;
		bool Execute(void) override;
		virtual void OnEvent(Event*) override{}

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		Lua* m_luaState;

	private:
	};


}