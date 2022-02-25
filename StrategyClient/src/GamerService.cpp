#include "GamerService.h"

GamerService* GamerService::g_gamerService = nullptr;

#ifdef PLATFORM_WINDOWS
// Implement SteamGamerService
struct GamerService::Impl
{
	Impl(){}
	~Impl(){}

	bool Initialize()
	{
		return true;
	}
	void Terminate()
	{
	}

	// ...
};
#elif PLATFORM_XBOX_ONE
// Implement XboxLiveGamerService
struct GamerService::Impl
{
	Impl();
	~Impl();

	// ...
};
#elif PLATFORM_NN_SWITCH
// Implement NintendoGamerService
struct GamerService::Impl
{
	Impl();
	~Impl();

	// ...
};
#else
#endif





// GAMERSERVICE
bool GamerService::Initialize()
{
	m_impl = new Impl();
	return m_impl->Initialize();
}

void GamerService::Terminate()
{
	m_impl->Terminate();
}