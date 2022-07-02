#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_
#pragma once

#include <vector>


/// @brief General purpose std::map like data storage structure
/// designed to be cache and run time optimized. The Key is always a String (const char*).
/// @tparam DATATYPE The datatype to be stored inside the map as the value.
template < class DATATYPE >
class CHashMap
{
public:
	CHashMap() = default;
	~CHashMap() = default;

	/// @brief Move data to be stored in the Hash Map. Generates a unique hash value for name, with with the data
	/// can be retrieved later.
	/// @param data Data to be stored.
	void AddData(const char* name, DATATYPE& data)
	{
		size_t hash = DJBHash(name, strlen(name));
		size_t index = m_data.size();

		m_indices.push_back({ hash, index });
		m_data.push_back(std::move(data));
	}

	/// @brief Retrieve the stored data.
	/// @param name Name of the data to be retrieved. The data had to be stored under this name.
	/// @return Searched for Object. Undefined behavior if the name was not stored before searching.
	DATATYPE& RetrieveData(const char* name)
	{
		size_t hash = DJBHash(name, strlen(name));
		for(auto& index: m_indices)
		{
			if(index.first == hash)
			{
				return m_data[index.second];
			}
		}
	}

private:
	/// @brief Generate a hash value for input string.
	/// Algorithm by Prof. Daniel J. Bernstein. Source: https://www.partow.net/programming/hashfunctions/#StringHashing 06 - DJB Hash Function.
	/// @param str 
	/// @param length 
	/// @return 
	size_t DJBHash(const char* str, size_t length)
	{
		size_t hash = length;
		for(size_t i = 0; i < length; ++str, ++i)
		{
			hash = ((hash << 5) + hash) + (*str);
		}
		return hash;
	}

private:
	std::vector< std::pair< size_t, size_t > > m_indices;
	std::vector< DATATYPE > m_data;
};



#endif