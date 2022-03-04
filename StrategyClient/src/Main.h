
// COMMON INCLUDES
#include <memory>
#include <future>

#include "Platform.h"
#include "StateMachine.h"
#include "Localization.h"

// ENGINE
#include <CherrySoda/CherrySoda.h>
#include <CherrySoda/Main.h>


// SOUND

// EVENT SYSTEM
#include "EventSystem.h"
static size_t g_iEntityID = 0;

// NETWORKING
#include "Random.h"
#include "NetCommon.h"
#include <olcPGEX_Network.h>
#include "GamerService.h"
#include "PlatformClient.h"


#define MASTER_SERVER_PORT 60777
#define MASTER_SERVER_IP "100.81.190.242"
#define CLIENT_MAJOR_VERSION 0
#define CLIENT_MINOR_VERSION 1
#define CLIENT_REVIEW_VERSION 0
#ifdef _DEBUG
#define CLIENT_TITLE_STRING "Strategy Prototype"
#else
#define CLIENT_TITLE_STRING "Strategy"
#endif



bool TEST_EmitEvent()
{
	return true;
}


// GAME SCENES
#include "InitializationScene.h"
#include "SplashScreenScene.h"



class App : public olc::net::client_interface< net::Message >,	// Networking Client.
			public cherrysoda::Engine							// Engine Client.
{

	// SCENES MAY NEED TO ACCESS APP DATA.
	friend class InitializationScene;
	friend class SplashSceenScene;

public:

	typedef cherrysoda::Engine base;

	// ENGINE OVERRIDES
	App();
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

	// MISC SUBSYSTEMS
	bool InitializeLocalization();
	void TerminateLocalization();

private:

	// Font
	cherrysoda::PixelFont* m_font = nullptr;

	// Networking and Multiplayer related Classes.
	net::UserDesc* m_localUserDesc = nullptr;
	
	net::NetGameobject* m_NetGameobject = nullptr;


	StateMachine* m_stateMachine = nullptr;


	cherrysoda::Image* m_Image;


	cherrysoda::MTexture* m_DefaultTexture;


	cherrysoda::STL::Map<cherrysoda::String, cherrysoda::BitTagValueType > m_entityNameTagMap;

private:
	// STEAM CALLBACK
	STEAM_CALLBACK(App, _onSteamGameOverlayActivated, GameOverlayActivated_t);

};