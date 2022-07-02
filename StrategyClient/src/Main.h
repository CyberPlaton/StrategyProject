
// COMMON INCLUDES
#include <memory>
#include <future>

// ENGINE
#include <CherrySoda/CherrySoda.h>
#include <CherrySoda/Main.h>

#include "Platform.h"
#include "StateMachine.h"
#include "Localization.h"
#include "Factory.h"


// LOGGING
#include "Logging.h"


// COMPONENTS
#include "CameraController.h"
#include "SelectableEntity.h"
#include "SelectableBuilding.h"
#include "SelectableUnit.h"
#include "Observable.h"
#include "Observer.h"
#include "AbilitiesDataMap.h"
#include "StatusEffectsDataMap.h"

// SOUND



// EVENT SYSTEM
#include "EventSystem.h"


// NETWORKING
#include "NetLib.h"
#include "ClientInterface.h"
#include "ServerInterface.h"
#include "Observer.h"
#include "GamerService.h"
#include "PlatformClient.h"

#define OFFLINE_GAME_CLIENT_TEST true
#define CONNECTION_TIMEOUT_TIMER_SECONDS 5
#define MASTER_SERVER_PORT 60777
#define MASTER_SERVER_IP "192.168.178.43"



// Create application Title Version string.
#define STRING(text) #text
#define CLIENT_TITLE_STRING Strategy v
#ifdef DEBUG
#define TITLE(title, major, minor, review) \
STRING(title) \
STRING(major) \
"." STRING(minor) \
"." STRING(review) \
"_" STRING(DEBUG)
#endif
#ifdef RELEASE
#define TITLE(title, major, minor, review) \
STRING(title) \
STRING(major) \
"." STRING(minor) \
"." STRING(review) \
"_" STRING(RELEASE)
#endif
#ifdef DISTR
#define TITLE(title, major, minor, review) \
STRING(title) \
STRING(major) \
"." STRING(minor) \
"." STRING(review)
#endif



bool TEST_EmitEvent()
{
	return true;
}



// RENDERER
#include "CustomRenderer.h"

// GAME SCENES
#include "InitializationScene.h"
#include "SplashScreenScene.h"
#include "DebugGameScene.h"


class App : public net::ClientInterface,						// Networking Client.
			public cherrysoda::Engine							// Engine Client.
{
	friend class cherrysoda::SceneGraphFactory;
	friend class cherrysoda::InitializationScene;
	friend class cherrysoda::SplashSceenScene;
	friend class cherrysoda::DebugGameScene;

public:
	typedef cherrysoda::Engine base;

	// ENGINE OVERRIDES
	App(uint64_t width, uint64_t height);
	void Update() override;
	void Initialize() override;
	void LoadContent() override;
	void Terminate() override;



	// NETWORKING AND MULTIPLAYER
	bool InitializePlatformClient();
	void TerminatePlatformClient();
	bool InitializeGamerService();
	void TerminateGamerService();
	bool InitializeMasterConnection();
	void TerminateMasterConnection();

	void OnConnected(RakNet::Packet* packet) override final;
	void OnDisconnected(RakNet::Packet* packet) override final;
	void OnMessage(RakNet::Packet* packet) override final;

	// SUBSYSTEMS
	bool InitializeLocalization();
	void TerminateLocalization();
	bool InitializeLogger(bool gamelog_open, bool limit_number_of_retained_messages, size_t count);
	void TerminateLogger();
	
	// UTIL
	void RegisterCommands();
	inline bool IsInitialized() { return m_initialized; }
	inline int64_t GetVersion() { return 1000; }

private:

	bool m_initialized = false;

	// Font
	cherrysoda::PixelFont* m_font = nullptr;


	// Networking and Multiplayer related Classes.
	net::SClientDescription * m_localClientDesc = nullptr;


	cherrysoda::CStateMachine* m_stateMachine = nullptr;


	cherrysoda::Image* m_Image;


	cherrysoda::MTexture* m_DefaultTexture;


	cherrysoda::STL::Map<cherrysoda::String, cherrysoda::BitTagValueType > m_entityNameTagMap;

private:
	// STEAM CALLBACK
	STEAM_CALLBACK(App, _onSteamGameOverlayActivated, GameOverlayActivated_t);

};
