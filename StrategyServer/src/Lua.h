#pragma once

// LUABRIDGE
#include "LuaLibrary.h"
#include "LuaBridge.h"
#include <LuaBridge/Array.h>
#include <LuaBridge/List.h>
#include <LuaBridge/Map.h>
#include <LuaBridge/UnorderedMap.h>
#include <LuaBridge/Vector.h>

#include <exception>


namespace cherrysoda
{
	/*
	class Lua;
	class LuaScript;

	class LuaFactory
	{
	public:
		static Lua* InstantiateLua();
		static void TerminateLua(Lua* lua);
		static void InitializeBinding(Lua* lua) {};
		static bool LoadScript(Lua* lua);

	private:

	private:

	};


	class Lua
	{
		friend class LuaFactory;
	public:
		Lua() = default;
		~Lua() = default;

		bool TryLoad(Lua* lua);
		bool TryExecute(const std::string& function_name);

	private:
		lua_State* m_luaState = nullptr;

	private:
		bool Initialize();
		void Terminate();

	};
	

	class LuaScript
	{
		friend class Lua;
	public:
		LuaScript(const std::string& filepath): m_script(filepath) {}


	private:
		std::string m_errorMessage;
		std::string m_script;


	private:
	};
	*/
}