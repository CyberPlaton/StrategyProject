#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>
#include <list>
#include <map>
#include <mutex>
#include <any>
#include <chrono>
#include <iostream>
#include <queue>

#define STATIC_GET_DEL(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS();} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}


namespace bt
{
	class BTNode;

	size_t DJBHash(const char* str, size_t length)
	{
		size_t hash = length;
		for (size_t i = 0; i < length; ++str, ++i)
		{
			hash = ((hash << 5) + hash) + (*str);
		}
		return hash;
	}

	class BTCurrentRunningNodeMngr
	{
	public:
		STATIC_GET_DEL(BTCurrentRunningNodeMngr, g_BTCurrentRunningNodeMngr);

		static BTNode* GetRunning(size_t behavior_tree_hash)
		{
			if(m_RunningNodeMap.find(behavior_tree_hash) != m_RunningNodeMap.end())
			{
				return m_RunningNodeMap[behavior_tree_hash];
			}
			return nullptr;
		}
		static void SetRunning(size_t behavior_tree_hash, BTNode* behavior_tree_node)
		{
			m_RunningNodeMap[behavior_tree_hash] = behavior_tree_node;
		}


	private:
		static BTCurrentRunningNodeMngr* g_BTCurrentRunningNodeMngr;
		static std::map< size_t, BTNode* > m_RunningNodeMap;

	};


	__declspec(selectany) bt::BTCurrentRunningNodeMngr* BTCurrentRunningNodeMngr::g_BTCurrentRunningNodeMngr = nullptr;
	__declspec(selectany) std::map< size_t, BTNode* > BTCurrentRunningNodeMngr::m_RunningNodeMap;

}