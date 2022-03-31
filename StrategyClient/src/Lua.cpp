#include "Lua.h"

namespace cherrysoda
{
	Lua* LuaFactory::InstantiateLua()
	{
		auto lua = new Lua();
		if (lua->Initialize())
		{
			InitializeBinding(lua);
			return lua;
		}
		else
		{
			TerminateLua(lua);
			return nullptr;
		}
	}
	void LuaFactory::TerminateLua(Lua* lua)
	{
		if (lua)
		{
			lua->Terminate();
			delete lua; lua = nullptr;
		}
	}
	void LuaFactory::InitializeBinding(Lua* lua)
	{

	}
	bool LuaFactory::LoadScript(ScriptInterface* script, Lua* lua)
	{
		return lua->TryLoad(script, lua);
	}

	bool Lua::TryLoad(ScriptInterface* script, Lua* lua)
	{
		int result;
		String error;

		try
		{
			if (result = luaL_dofile(lua->m_luaState, script->m_filepath.c_str()); result == LUA_OK)
			{
				script->Loaded(true);
				return true;
			}
			else
			{
				throw luabridge::LuaException(lua->m_luaState, result);
			}
		}
		catch (luabridge::LuaException& err)
		{
			script->Error(err.what());
			script->Loaded(false);
			return false;
		}
	}
	bool Lua::TryExecute(ScriptInterface* script, String function_name)
	{
		if (!script->Loaded())
		{
			script->Error("Not Loaded");
			return false;
		}
		
		try
		{
			// Run scripts Main function and return result.
			auto func = luabridge::getGlobal(m_luaState, function_name.c_str());
			func();
			luabridge::Stack< bool > s;
			return s.get(m_luaState, 0);
		}
		catch (luabridge::LuaException& err)
		{
			script->m_error = err.what();
			return false;
		}
	}
	bool Lua::Initialize()
	{
		m_luaState = luaL_newstate();
		luaL_openlibs(m_luaState);
		return m_luaState != nullptr;
	}
	void Lua::Terminate()
	{
		if (m_luaState)
		{
			lua_close(m_luaState);
			m_luaState = nullptr;
		}
	}
}