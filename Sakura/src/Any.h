#ifndef _ANY_H_
#define _ANY_H_
#pragma once

#include "BTCommon.h"

namespace sakura
{
	class Any
	{
	public:
		Any(std::any& value, const std::string& type) : m_Value(std::move(value)), m_Type(type)
		{
		}
		Any(std::any&& value, const std::string& type) : m_Value(value), m_Type(type)
		{
		}
		Any() : m_Type("none")
		{
		}
		~Any()
		{
			m_Value.reset();
			m_Type.clear();
		}

		/// @brief Get the underlying Type of the Data.
		/// @return The Type.
		std::string Type() { return m_Type; }

		/// @brief Whether the Object has a value set.
		/// @return True if Object has a value set.
		bool HasValue()
		{
			if(m_Type.compare("none") == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		/// @brief Get the stored value as plain-old-data. E.g. a float or string.
		/// @tparam T The type that the data will be casted to.
		/// @return The data as given data type.
		template < typename T >
		T AsPOD()
		{
			try
			{
				return std::any_cast<T>(m_Value);
			}
			catch (...)
			{
				// Silently Ignore the Error.
			}
		}

		/// @brief Get the stored value as a pointer to an object.
		/// @tparam T The class type of the Object.
		/// @return Pointer to the Object.
		template < typename T >
		T* AsObject()
		{
			try
			{
				return std::any_cast<T*>(m_Value);
			}
			catch (...)
			{
				// Silently Ignore the Error.
				// Return an invalid value.
				return nullptr;
			}
		}

		/// @brief Store plain-old-data value.
		/// @tparam T Data type.
		/// @param value The value itself.
		/// @param type Type of the value as string.
		template < typename T >
		void SetPOD(T value, const std::string& type)
		{
			m_Value.reset();
			m_Value = std::any(value);
			m_Type = type;
		}

		/// @brief Store a pointer to an Object.
		/// @tparam T The class type of the Object.
		/// @param value Pointer to the Object.
		/// @param type String name of the Class.
		template < typename T >
		void SetObject(T* value, const std::string& type)
		{
			m_Value.reset();
			m_Value = std::any(value);
			m_Type = type;
		}


	private:
		std::any m_Value;
		std::string m_Type;
	};
}

#endif