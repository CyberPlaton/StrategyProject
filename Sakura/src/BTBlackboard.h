#pragma once

#include "BTCommon.h"
#include "BTNodes.h"
#include "Any.h"

namespace sakura
{
	class BTBlackboard
	{
	public:
		BTBlackboard(std::string name) : m_Name(name) {}
		~BTBlackboard()
		{
			while (!m_Data.empty())
			{

				m_Data.erase(m_Data.begin());
			}

			m_Data.clear();
			m_Name.clear();
		}


		template < typename T >
		void SetDataPOD(std::string name, T value, const std::string& type)
		{
			// Does the entry already exist
			if (m_Data.find(name) != m_Data.end())
			{
				// Change entry
				m_Data[name].SetPOD<T>(value, type);
			}
			else
			{
				// Create new entry
				Any a(value, name);
				m_Data.emplace(name, a);
			}
		}

		template < typename T >
		void SetDataObject(std::string name, T* value, const std::string& type)
		{
			// Does the entry already exist
			if (m_Data.find(name) != m_Data.end())
			{
				// Change entry
				m_Data[name].SetObject<T>(value, type);
			}
			else
			{
				// Create new object pointer entry
				Any a(value, type);
				m_Data.emplace(name, a);
			}
		}


		/// @brief Get data as plain-old-data. E.g. a float or string.
		/// @tparam T Data type of the value.
		/// @param name Name under which the value is stored.
		/// @return A copy of the value. If the supplied is invalid, nothing is returned and the behavior is undefined.
		template < typename T >
		T GetDataPOD(const std::string& name)
		{
			for (auto& it : m_Data)
			{
				if (it.first.compare(name) == 0)
				{
					return it.second.AsPOD<T>();
				}
			}
		}

		/// @brief Get data to a Pointer. If it does not exists in the Blackboard a nullptr is returned.
		/// @tparam T Class type of the Object.
		/// @param name Name under which the Object Pointer is stored.
		/// @return Pointer to Object.
		template < typename T >
		T* GetDataObject(const std::string& name)
		{
			for (auto& it : m_Data)
			{
				if (it.first.compare(name) == 0)
				{
					return it.second.AsObject<T>();
				}
			}
			return nullptr;
		}


		std::string Name() { return m_Name; }

	private:
		std::string m_Name;
		std::map<std::string, Any> m_Data;
	};

}