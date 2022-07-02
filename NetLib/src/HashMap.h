
#include <vector>


template < class DATATYPE >
class CHashMap
{
public:
	CHashMap() = default;

	/// @brief Push new data into vector.
	/// @param data 
	void AddData(const std::string& name, DATATYPE& data)
	{
		size_t hash = Hash(name);
		m_data.push_back(hash, std::move(data));
	}


	/// @brief Fill a structure with data found by name.
	/// @param name 
	/// @param fillable 
	void RetrieveData(const std::string& name, DATATYPE& fillable)
	{
		size_t hash = Hash(name); // O(Log n).

		for(auto& d: m_data) // O(n)
		{
			if(d.first == hash)
			{
				// Fill fillable.
				Fill(d.second, fillable);
				return;
			}
		}

		// O(n), but cache "optimized", so we can load as much of 
		// DATATYPE in one cache line as possible, thus making it much faster.
	}




	std::vector< std::pair< size_t, DATATYPE > > m_data;
};