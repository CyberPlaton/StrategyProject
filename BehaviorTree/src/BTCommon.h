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

	/// @brief Create a Hash Value for a String.
	/// Algorithm by Prof. Daniel J. Bernstein. Source: https://www.partow.net/programming/hashfunctions/#StringHashing 06 - DJB Hash Function.
	/// @param str C String to be hashed.
	/// @param length Length of the C String.
	/// @return The generated Hash Value.
	static size_t DJBHash(const char* str, size_t length)
	{
		size_t hash = length;
		for (size_t i = 0; i < length; ++str, ++i)
		{
			hash = ((hash << 5) + hash) + (*str);
		}
		return hash;
	}


	/// @brief Datastructure responsible for storing the last executed node for a Behavior Tree.
	//	The last running node has to be set manually by the designer in the nodes OnUpdate function, else nothing is 
	//	stored and the Behavior Tree executes from the Root like normal.
	class BTCurrentRunningNodeMngr
	{
	public:
		STATIC_GET_DEL(BTCurrentRunningNodeMngr, g_BTCurrentRunningNodeMngr);


		/// @brief Query whether the Behavior Tree has a node set as last one running.
		/// @param behavior_tree_hash The Hash of the Behavior Tree name.
		/// @return Either Pointer to Node if one was set, or nullptr.
		static BTNode* GetRunning(size_t behavior_tree_hash)
		{
			if(m_RunningNodeMap.find(behavior_tree_hash) != m_RunningNodeMap.end())
			{
				return m_RunningNodeMap[behavior_tree_hash];
			}
			return nullptr;
		}

		/// @brief Set the last running node for the Behavior Tree.
		/// @param behavior_tree_hash Hash of the name of the Behavior Tree.
		/// @param behavior_tree_node Pointer to the last running Node.
		static void SetRunning(size_t behavior_tree_hash, BTNode* behavior_tree_node)
		{
			m_RunningNodeMap[behavior_tree_hash] = behavior_tree_node;
		}

		/// @brief Reset the last runnning node; meaning remove it from storage.
		/// @param behavior_tree_hash Hash of the Name of the Behavior Tree.
		static void ResetRunning(size_t behavior_tree_hash)
		{
			m_RunningNodeMap[behavior_tree_hash] = nullptr;
		}


	private:
		static BTCurrentRunningNodeMngr* g_BTCurrentRunningNodeMngr;
		static std::map< size_t, BTNode* > m_RunningNodeMap;

	};


	__declspec(selectany) bt::BTCurrentRunningNodeMngr* BTCurrentRunningNodeMngr::g_BTCurrentRunningNodeMngr = nullptr;
	__declspec(selectany) std::map< size_t, BTNode* > BTCurrentRunningNodeMngr::m_RunningNodeMap;

}