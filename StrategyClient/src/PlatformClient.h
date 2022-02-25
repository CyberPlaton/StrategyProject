#pragma once

#include <string>
#include <CherrySoda/Util/Log.h>
#include <CherrySoda/Util/String.h>

// STEAM
#include <steam_api.h>

#include "NetCommon.h"

#define STATIC_GET_DEL(CLASS, STATIC_MEMBER) \
static CLASS* get() \
{ if(!STATIC_MEMBER){STATIC_MEMBER = new CLASS();} return STATIC_MEMBER;} \
static void del() \
{if(STATIC_MEMBER){delete STATIC_MEMBER; STATIC_MEMBER = nullptr;}}


// The Client has common function with special implementation
// based on which gaming platform we are, like Steam or Switch.
// 
// Thus, to some extent we can rely on Client validation of those platforms...
class PlatformClient
{
public:
	// Forward Declaration.
	struct Impl;

public:

	STATIC_GET_DEL(PlatformClient, g_platformClient);

	bool Initialize();
	void Terminate();

	// Get the current Platform the client is running on.
	net::EUserPlatform GetClientPlatform();

	// Get the local user id based on current platform like Steam or Switch.
	size_t GetClientPlatformID();

	// Verify current client as valid and let it run,
	// meaning allow further connection attempts to the server.
	bool AuthenticatePlatformClient();

	// Get the local user name based on the Platform like Steam or Switch.
	cherrysoda::String GetClientPlatformName();

private:

	static PlatformClient* g_platformClient;

	Impl* m_impl;


#if defined(PLATFORM_WINDOWS)
#elif defined(PLATFORM_XBOX_ONE)
#elif defined(PLATFORM_NN_SWITCH)
#elif defined(PLATFORM_PS)
#endif


private:

	PlatformClient() : m_impl(nullptr) {}
	PlatformClient(const PlatformClient&) = delete;
	PlatformClient& operator=(const PlatformClient&) = delete;
	~PlatformClient() {}
};