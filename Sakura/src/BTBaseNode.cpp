#include "BTBaseNode.h"

namespace sakura
{

	BTBlackboard* BTBaseNode::GlobalBlackboard()
	{
		// If we have a Parent, then we are not Root.
		if(m_Parent)
		{
			return m_Parent->GlobalBlackboard();
		}

		// If we are Root, we have to check whether a Blackboard was set.
		if(m_GlobalBlackboard)
		{
			return m_GlobalBlackboard;
		}

		// None was set. Return nullptr.
		return nullptr;
	}

	void BTBaseNode::GlobalBlackboard(BTBlackboard* b)
	{
		m_GlobalBlackboard = b;
	}

}