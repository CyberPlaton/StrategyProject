#include "NetCommon.h"

namespace net
{
	bool UserDesc::operator==(const UserDesc& other)
	{
		// Add more checks as needed...
		return (
			m_uuid == other.m_uuid &&
			m_steamId == other.m_steamId &&
			m_nintendoId == other.m_nintendoId &&
			m_currency == other.m_currency &&
			strcmp(m_steamName.c_str(), other.m_steamName.c_str()) == 0 &&
			strcmp(m_nintendoName.c_str(), other.m_nintendoName.c_str()) == 0 &&
			m_userPlatform == other.m_userPlatform
			);
	}
}