#pragma once

#include "BTBaseNode.h"

namespace bt
{
	/// @brief TODO.
	class BTDecorator : public BTBaseNode
	{
	public:
		BTDecorator(std::string name) : BTBaseNode(name)
		{
		}
		virtual ~BTDecorator()
		{
			delete m_Child; m_Child = nullptr;
		}


		// TODO: REQUIRES A RETURN VALUE MODIFICATION FUNCTION.


		//////////////////////////////
		// Decorator Base functionality.
		//////////////////////////////
		BTNode* Child(std::string) override { return m_Child; }
		std::vector<BTNode*>& Children() override { std::vector<BTNode*> v; v.push_back(m_Child); return v; }
		void AddChild(BTNode* child) override { m_Child = child; }
		void RemoveChild(std::string name) override 
		{
			if (m_Child->Name().compare(name) == 0) delete m_Child; m_Child = nullptr;
		}
		void RemoveFirstChild() override { RemoveChild(m_Child->Name()); }
		std::string Type() override{return "BTDecorator";}
	protected:
		BTNode* m_Child = nullptr;
	};
}