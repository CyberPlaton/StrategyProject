#include "Lua.h"

namespace cherrysoda
{
	/*
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
	bool LuaFactory::LoadScript(Lua* lua)
	{
		return lua->TryLoad(lua);
	}

	bool Lua::TryLoad(Lua* lua)
	{
		int result;
		std::string error;

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
	bool Lua::TryExecute(const std::string& function_name)
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
	*/
}