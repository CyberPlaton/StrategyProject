#include "PlatformClient.h"

PlatformClient* PlatformClient::g_platformClient = nullptr;

#if defined(PLATFORM_WINDOWS)
struct PlatformClient::Impl
{
	// Steam Platform Local User Information.
	CSteamID m_localUser;
	size_t m_localUserID;
	cherrysoda::String m_localUserName;
	
	Impl(){}
	~Impl(){}

	bool Initialize()
	{
		if (SteamAPI_RestartAppIfNecessary(480))
		{
			return false;
		}
		if (!SteamAPI_Init())
		{
			return false;
		}

		if (!AuthenticatePlatformClient())
		{
			return false;
		}

		m_localUser = SteamUser()->GetSteamID();
		m_localUserID = m_localUser.ConvertToUint64();
		m_localUserName = SteamFriends()->GetPersonaName();

		return true;
	}
	void Terminate()
	{
	}

	size_t PlatformClient::Impl::GetClientPlatformID()
	{
		return m_localUserID;
	}
	bool PlatformClient::Impl::AuthenticatePlatformClient()
	{
		return true;
	}
	cherrysoda::String PlatformClient::Impl::GetClientPlatformName()
	{
		return m_localUserName;
	}
	net::EUserPlatform PlatformClient::Impl::GetClientPlatform()
	{
		return net::EUserPlatform::UP_STEAM;
	}

};

#elif defined(PLATFORM_XBOX_ONE)
#elif defined(PLATFORM_NN_SWITCH)
#elif defined(PLATFORM_PS)
#endif


// CLIENT
bool PlatformClient::Initialize()
{
	m_impl = new Impl();
	return m_impl->Initialize();
}
void PlatformClient::Terminate()
{
	m_impl->Terminate();
}
size_t PlatformClient::GetClientPlatformID()
{
	return m_impl->GetClientPlatformID();
}
bool PlatformClient::AuthenticatePlatformClient()
{
	return m_impl->AuthenticatePlatformClient();
}
cherrysoda::String PlatformClient::GetClientPlatformName()
{
	return m_impl->GetClientPlatformName();
}
net::EUserPlatform PlatformClient::GetClientPlatform()
{
	return m_impl->GetClientPlatform();
}