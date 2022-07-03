#pragma once

#include "NetCommon.h"
#include "CommonDefines.h"

// Forward decl.
extern class App;

/// @brief CNetCommMngr is intended to dispatch Game Object updates to MasterServer,
/// also it allows to send trivial messages to MasterServer without requiring to interface with the App class.
class CNetCommMngr
{
public:
	STATIC_GET_DEL(CNetCommMngr, g_NetCommMngr);

	/// @brief Store the App class.
	/// @param app The App class object.
	/// @return True on success.
	static bool Initialize(App* app);
	
	/// @brief Store the MasterServer address for later network messages.
	/// @param address The MasterServer system address.
	/// @return True on success.
	static bool InitializeMasterServerConnection(RakNet::SystemAddress address);
	static void Terminate();

	/// @brief Dispatch a newer version of the net::SGameobject state to MasterServer, which will be updated in Database.
	/// @param object The object to be updated.
	static void UpdateNetGameobject(net::SGameobject* object);

	/// @brief Create a message to be sent to the MasterServer.
	/// @param id The message id of the message to be sent.
	static void CreateAndDispatchMasterServerMessage(net::EMessageId id);

private:
	static CNetCommMngr* g_NetCommMngr;
	static App* g_App;
	static RakNet::SystemAddress g_masterServerAddress;

private:
	CNetCommMngr() = default;
};