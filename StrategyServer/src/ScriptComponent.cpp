#include "ScriptComponent.h"

namespace cherrysoda
{
	/*
	bool ScriptComponent::Initialize(const String& filepath)
	{
		if (m_luaState = LuaFactory::InstantiateLua(); m_luaState != nullptr)
		{
			m_filepath = filepath;
			if (LuaFactory::LoadScript(this, m_luaState))
			{
				return true;
			}
		}

		return false;
	}
	void ScriptComponent::Terminate()
	{
		LuaFactory::TerminateLua(m_luaState);
	}
	bool ScriptComponent::Execute(void)
	{
		return m_luaState->TryExecute(this, "Main");
	}
	bool ScriptComponent::OnEntityTick(float dt)
	{
		return m_luaState->TryExecute(this, "Main");
	}
	void ScriptComponent::OnEntityCreate()
	{
		m_luaState->TryExecute(this, "Begin");
	}
	void ScriptComponent::OnEntityDestroy()
	{
		m_luaState->TryExecute(this, "End");
	}
	void ScriptComponent::OnEntityUpdate()
	{
		Execute();
	}
	*/
}