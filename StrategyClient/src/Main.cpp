#include "Main.h"

/*
// The scripting interface for the client will be removed.
// This is a merely example of how to use functionality created previously,
// so it will be easier to port it to the game server.
void cherrysoda::LuaFactory::InitializeBinding(cherrysoda::Lua* lua)
{
	using namespace cherrysoda;
	using namespace luabridge;

	auto state = lua->m_luaState;

	REGISTER_CLASS_FUNCTION_TO_LUA(state, "Engine", App, SetTitle);
	REGISTER_CLASS_FUNCTION_TO_LUA(state, "Engine", App, GetWindowHeight);
	REGISTER_CLASS_FUNCTION_TO_LUA(state, "Engine", App, GetWindowWidth);
	REGISTER_CLASS_FUNCTION_TO_LUA(state, "Engine", App, SetFullscreen);
	REGISTER_CLASS_FUNCTION_TO_LUA(state, "Engine", App, SetWindowed);
	REGISTER_CLASS_FUNCTION_TO_LUA(state, "Engine", App, FPS);
	REGISTER_CLASS_FUNCTION_TO_LUA(state, "Engine", App, DeltaTime);

}
*/


void App::OnConnected(RakNet::Packet* packet)
{
	// Do nothing for Application.
	// Scenes handle the networking traffic.
}
void App::OnDisconnected(RakNet::Packet* packet)
{
	// Do nothing for Application.
	// Scenes handle the networking traffic.
}
void App::OnMessage(RakNet::Packet* packet)
{
	// Do nothing for Application.
	// Scenes handle the networking traffic.
}


bool cherrysoda::SceneGraphFactory::LoadSceneGraph(const cherrysoda::String& filename, App* app)
{
	using namespace cherrysoda;
	
	XMLUtil xml;
	cherrysoda::xml::XMLDocument doc;
	if (xml.ReadXMLFile(doc, filename))
	{
		app->m_stateMachine = new CStateMachine(app);


		auto root = doc.RootElement();
		auto scene = root->FirstChildElement("Scene");
		while(scene)
		{
			State* new_scene = nullptr;

			auto name = scene->FirstChildElement("Name")->GetText();
			auto initial = scene->FirstChildElement("Initial")->BoolText();


			LOG_DBG_INFO("[{:.4f}][SceneGraphFactory::LoadSceneGraph] Loading Scene: \"{}\" from \"{}\"!", APP_RUN_TIME, name, filename.c_str());
			LOG_GAME_INFO("[%.4f][SceneGraphFactory::LoadSceneGraph] Loading Scene: \"%s\" from \"%s\"!", APP_RUN_TIME, name, filename.c_str());

			// TODO
			// Create Scene. Very Sad...
			if (strcmp(name, "Initialization") == 0)
			{
				new_scene = new InitializationScene(name, app->m_stateMachine, app);
			}
			else if(strcmp(name, "SplashScreen") == 0)
			{
				new_scene = new SplashSceenScene(name, app->m_stateMachine, app);
			}
			else if (strcmp(name, "DebugGame") == 0)
			{
				new_scene = new DebugGameScene(name, app->m_stateMachine, app);
			}
			else
			{
				LOG_DBG_CRITICAL("[{:.4f}][SceneGraphFactory::LoadSceneGraph] Could not load unknown scene type: \"{}\"!", APP_RUN_TIME, name);
				LOG_FILE_CRITICAL("[{:.4f}][SceneGraphFactory::LoadSceneGraph] Could not load unknown scene type: \"{}\"!", APP_RUN_TIME, name);
				return false;
			}


			// Set as starting scene if declared.
			if (initial)
			{
				app->m_stateMachine->SetInitialState(new_scene);
				app->SetScene(new_scene->AsScene());
			}


			auto transit = scene->FirstChildElement("Transit");
			while (transit)
			{
				auto transit_to = transit->GetText();

				new_scene->AddTransition(transit_to);

				transit = transit->NextSiblingElement("Transit");
			}

			scene = scene->NextSiblingElement("Scene");
		}

		return true;
	}

	return false;
}

void CNetCommMngr::UpdateNetGameobject(net::SGameobject* object)
{
	LOG_GAME_INFO("[%.4f][CNetCommMngr::UpdateNetGameobject] Dispatch to MS \"%s\"!", APP_RUN_TIME, object->m_name.C_String());

	/*
	olc::net::message< net::Message > out;
	out.header.id = net::Message::NET_MSG_GAMEOBJECT_UPDATE;
	out << object;

	g_App->Send(out);
	*/
}

void CNetCommMngr::CreateAndDispatchMasterServerMessage(net::EMessageId id)
{
	CREATE_MESSAGE(out, id);
	g_App->Send(out, g_masterServerAddress);
}

bool CNetCommMngr::InitializeMasterServerConnection(RakNet::SystemAddress address)
{
	std::string addr = address.ToString(true, ':');
	
	LOG_GAME_INFO("[%.4f][CNetCommMngr::InitializeMasterServerConnection] Storing MS Address: \"%s\"", APP_RUN_TIME, addr.c_str());
	LOG_DBG_INFO("[{:.4f}][CNetCommMngr::InitializeMasterServerConnection] Storing MS Address: \"%s\"", APP_RUN_TIME, addr.c_str());
	LOG_FILE_INFO("[{:.4f}][CNetCommMngr::InitializeMasterServerConnection] Storing MS Address: \"%s\"", APP_RUN_TIME, addr.c_str());

	g_masterServerAddress = address;

	return true;
}

App::App(uint64_t width, uint64_t height) : base(width, height, "Empty Title"),
m_DefaultTexture(nullptr), m_Image(nullptr)
{
	SetTitle(TITLE(CLIENT_TITLE_STRING, 0, 1, 0));
	SetClearColor(cherrysoda::Color::Gray);

	// Hide Windows Console for Release.
#if defined PLATFORM_WINDOWS
	#if defined RELEASE || defined DISTR
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif
#endif
}
void App::Update()
{
	using namespace std;
	
	// ENGINE UPDATE
	base::Update();
	
	// STEAM UPDATE
	SteamAPI_RunCallbacks();

	// CLIENT UPDATE
	ClientInterface::Update(1 / 240);

	// LOGGER UPDATE
	cherrysoda::Logger::Update();
}

void App::Initialize()
{
	if (!InitializeLogger(true, false, 256))
	{
		LOG_DBG_CRITICAL("[{:.4f}][App::Initialize] Logger not initialized!", APP_RUN_TIME);
		LOG_FILE_CRITICAL("[{:.4f}][App::Initialize] Logger not initialized!", APP_RUN_TIME);
		base::Exit(); 
		return;
	}
	LOG_DBG_INFO("[{:.4f}][App::Initialize] Logger initialized...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Initialize] Logger initialized...", APP_RUN_TIME);


	if (!InitializeLocalization())
	{
		LOG_DBG_CRITICAL("[{:.4f}][App::Initialize] Localization not initialized!", APP_RUN_TIME);
		LOG_FILE_CRITICAL("[{:.4f}][App::Initialize] Localization not initialized!", APP_RUN_TIME);
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}][App::Initialize] Localization initialized...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Initialize] Localization initialized...", APP_RUN_TIME);


	// Initialize the Platform Client.
	if (!InitializePlatformClient())
	{
		LOG_DBG_CRITICAL("[{:.4f}][App::Initialize] Platform client not initialized!", APP_RUN_TIME);
		LOG_FILE_CRITICAL("[{:.4f}][App::Initialize] Platform client not initialized!", APP_RUN_TIME);
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}][App::Initialize] Platform client initialized...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Initialize] Platform client initialized...", APP_RUN_TIME);


	// Initialize the GamerService.
	if (!InitializeGamerService())
	{
		LOG_DBG_CRITICAL("[{:.4f}][App::Initialize] Gamer service not initialized!", APP_RUN_TIME);
		LOG_FILE_CRITICAL("[{:.4f}][App::Initialize] Gamer service not initialized!", APP_RUN_TIME);
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}][App::Initialize] Gamer service initialized...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Initialize] Gamer service initialized...", APP_RUN_TIME);

	if (!InitializeMasterConnection())
	{
		LOG_DBG_CRITICAL("[{:.4f}][App::Initialize] Master connection not initialized!", APP_RUN_TIME);
		LOG_FILE_CRITICAL("[{:.4f}][App::Initialize] Master connection not initialized!", APP_RUN_TIME);
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}][App::Initialize] Master connection initialized...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Initialize] Master connection initialized...", APP_RUN_TIME);


	if (!CNetCommMngr::Initialize(this))
	{
		LOG_DBG_CRITICAL("[{:.4f}][App::Initialize] Net communication manager not initialized!", APP_RUN_TIME);
		LOG_FILE_CRITICAL("[{:.4f}][App::Initialize] Net communication manager not initialized!", APP_RUN_TIME);
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}][App::Initialize] Net communication manager initialized...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Initialize] Net communication manager initialized...", APP_RUN_TIME);


	LOG_DBG_INFO("[{:.4f}][App::Initialize] Subsystems initialized!", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Initialize] Subsystems initialized!", APP_RUN_TIME);



	// Everything went OK. Startup the engine!
	base::ExitOnEscapeKeypress(false);
	base::Initialize();

	// Change GameLog ImGui color scheme.
	ImGui::StyleColorsDark();

	// Load Game Scenes!
	if (!cherrysoda::SceneGraphFactory::LoadSceneGraph("assets/SceneGraph.xml", this))
	{
		LOG_DBG_CRITICAL("[{:.4f}][App::Initialize] Failed loading Scene Graph \"{}\"!", APP_RUN_TIME, "assets/SceneGraph.xml");
		LOG_FILE_CRITICAL("[{:.4f}][App::Initialize] Failed loading Scene Graph \"{}\"!", APP_RUN_TIME, "assets/SceneGraph.xml");
		base::Exit();
		return;
	}

	m_font = new cherrysoda::PixelFont("PixelFont");
	auto atlas = cherrysoda::Atlas::FromAtlas("assets/font/atlases.json");
	m_font->AddFontSize("assets/font/font_json.json", atlas);


	m_initialized = true;
}


void App::LoadContent()
{
	// Load only if Initialization went OK.
	if (!App::IsInitialized())
	{
		base::LoadContent();
	}
}


void App::Terminate()
{
	// DELETE AUDIO
	// ...

	// DELETE PLATFORMCLIENT
	LOG_DBG_INFO("[{:.4f}][App::Terminate] Terminating Platform client...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Terminate] Terminating Platform client...", APP_RUN_TIME);
	TerminatePlatformClient();

	// DELETE GAMERSERVICE
	LOG_DBG_INFO("[{:.4f}][App::Terminate] Terminating Gamer service...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Terminate] Terminating Gamer service...", APP_RUN_TIME);
	TerminateGamerService();

	// DELTET MASTERSERVER CONNECTION
	LOG_DBG_INFO("[{:.4f}][App::Terminate] Terminating Master connection...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Terminate] Terminating Master connection...", APP_RUN_TIME);
	TerminateMasterConnection();

	// DELETE STEAM
	LOG_DBG_INFO("[{:.4f}][App::Terminate] Terminating SteamAPI...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Terminate] Terminating SteamAPI...", APP_RUN_TIME);
	SteamAPI_Shutdown();

	// DELETE LOCALIZATION
	LOG_DBG_INFO("[{:.4f}][App::Terminate] Terminating Localization...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Terminate] Terminating Localization...", APP_RUN_TIME);
	TerminateLocalization();

	// DELETE LOG
	LOG_DBG_INFO("[{:.4f}][App::Terminate] Terminating Logger...", APP_RUN_TIME);
	LOG_FILE_INFO("[{:.4f}][App::Terminate] Terminating Logger...", APP_RUN_TIME);
	TerminateLogger();
}

bool App::InitializeLogger(bool gamelog_open, bool limit_number_of_retained_messages, size_t count)
{
	return cherrysoda::Logger::Initialize(gamelog_open, limit_number_of_retained_messages, count);
}
void App::TerminateLogger()
{
	cherrysoda::Logger::Terminate();
}
bool App::InitializePlatformClient()
{
	if (!App::IsInitialized())
	{
		if (PlatformClient::get()->Initialize())
		{
			return true;
		}

		PlatformClient::del();
	}
	return false;
}
void App::TerminatePlatformClient()
{
	PlatformClient::get()->Terminate();
}
bool App::InitializeGamerService()
{
	if (!App::IsInitialized())
	{
		if (GamerService::get()->Initialize())
		{
			return true;
		}

		GamerService::del();
	}
	return false;
}
void App::TerminateGamerService()
{
	GamerService::get()->Terminate();
}
bool App::InitializeMasterConnection()
{
	if (!App::IsInitialized())
	{
		LOG_GAME_INFO("[%.4f][App::InitializeMasterConnection] Connecting...", APP_RUN_TIME);

		if(!OFFLINE_GAME_CLIENT_TEST)
		{
			if (!ClientInterface::Initialize(MASTER_SERVER_PORT, MASTER_SERVER_IP))
			{
				LOG_GAME_CRITICAL("[%.4f][InitializationScene::InitializeMasterConnection] Could not initialize ClientInterface!", APP_RUN_TIME);
				LOG_DBG_CRITICAL("[{.4f}][InitializationScene::InitializeMasterConnection] Could not initialize ClientInterface!", APP_RUN_TIME);
				return false;
			}

			LOG_GAME_SUCCESS("[%.4f][App::InitializeMasterConnection] ClientInterface initialized!", APP_RUN_TIME);

			Timer timeout_timer;
			timeout_timer.StartTimer();

			while (!ClientInterface::Connected() && timeout_timer.SecondsElapsed() < CONNECTION_TIMEOUT_TIMER_SECONDS)
			{
				ClientInterface::Update();
			}

			return ClientInterface::Connected();
		}	
		else
		{
			LOG_GAME_WARN("[%.4f][App::InitializeMasterConnection] Connection skipped due to offline test!", APP_RUN_TIME);
			LOG_DBG_WARN("[{:.4f}][App::InitializeMasterConnection] Connection skipped due to offline test!", APP_RUN_TIME);
			return true;
		}
	}

	return false;
}
void App::TerminateMasterConnection()
{
#ifndef OFFLINE_GAME_CLIENT_TEST
	ClientInterface::Terminate();
#endif
}


void App::_onSteamGameOverlayActivated(GameOverlayActivated_t* pCallback)
{
	if (pCallback->m_bActive)
	{
		printf_s("Steam overlay active\n");
	}
	else
	{
		printf_s("Steam overlay now inactive\n");
	}
}

void cherrysoda::CStateMachine::Transit(const cherrysoda::String& name)
{
	if (cherrysoda::STL::Find(m_currentState->m_transitions, name) != m_currentState->m_transitions.end())
	{
		// Delete previous state.
		delete m_previousState;
		m_previousState = nullptr;

		m_previousState = m_currentState;
		m_currentState = m_states[name];

		m_application->SetScene(m_currentState->AsScene());
	}
}

bool App::InitializeLocalization()
{
	if (Localization::Initialize())
	{
		return true;
	}
}
void App::TerminateLocalization()
{
	Localization::del();
}
void App::RegisterCommands()
{
#if defined DEBUG || RELEASE
	cherrysoda::CommandRegisterHelper("TestCommand", TestCommandBatchFunction, "No Help");
#endif
}


// SCENE FUNCTION IMPLEMENTATIONS
void cherrysoda::InitializationScene::SceneImpl::Begin()
{
	LOG_GAME_INFO("[%.4f][InitializationScene::Begin] ... ", APP_RUN_TIME);
}
void cherrysoda::InitializationScene::SceneImpl::End()
{
	LOG_GAME_INFO("[%.4f][InitializationScene::End] ... ", APP_RUN_TIME);
}
void cherrysoda::InitializationScene::SceneImpl::Update()
{
	static bool show_demo;
	ImGui::ShowDemoWindow(&show_demo);

	if(OFFLINE_GAME_CLIENT_TEST)
	{
		LOG_GAME_WARN("[%.4f][InitializationScene::Update] Debug: Transit to DebugGameScene", APP_RUN_TIME);
		LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] Debug: Transit to DebugGameScene", APP_RUN_TIME);
		m_stateMachine->Transit("DebugGame");
		return;
	}

	InitializationScene* initialization_scene = reinterpret_cast<InitializationScene*>(GetState());

	if (m_application->Connected() && m_initializationComplete == false)
	{
		auto packet = m_application->PopNextMessage();
		if (packet)
		{
			RakNet::MessageID id = (RakNet::MessageID)packet->data[0];

			switch (id)
			{
			case net::EMessageId::NET_MSG_REQUEST_USER_VALIDATION_DATA:
			{
				LOG_GAME_WARN("[%.4f][InitializationScene::Update] Validation Data requested!", APP_RUN_TIME);
				LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] Validation Data requested!", APP_RUN_TIME);

				net::SClientDescription clientDesc;

				auto platform = PlatformClient::get();
				clientDesc.m_platform = platform->GetClientPlatform();
				switch (clientDesc.m_platform)
				{
					case net::EClientPlatform::UP_STEAM:
					{
						clientDesc.m_steamName = platform->GetClientPlatformName().c_str();
						clientDesc.m_steamId = platform->GetClientPlatformID();
						break;
					}
					case net::EClientPlatform::UP_XBOX:
					{
						clientDesc.m_xboxLiveName = platform->GetClientPlatformName().c_str();
						clientDesc.m_xboxLiveId = platform->GetClientPlatformID();
						break;
					}
					case net::EClientPlatform::UP_SWITCH:
					{
						clientDesc.m_nintendoName = platform->GetClientPlatformName().c_str();
						clientDesc.m_nintendoId = platform->GetClientPlatformID();
						break;
					}
					case net::EClientPlatform::UP_PS:
					{
						clientDesc.m_psnName = platform->GetClientPlatformName().c_str();
						clientDesc.m_psnId = platform->GetClientPlatformID();
						break;
					}
				}

				clientDesc.m_version = m_application->GetVersion();

				CREATE_MESSAGE(out, net::EMessageId::NET_MSG_USER_VALIDATION_DATA);
				clientDesc.Serialize(out);
				m_application->Send(out, packet->systemAddress);
				break;
			}
			
			case net::EMessageId::NET_MSG_CLIENT_REJECT:
			{
				LOG_GAME_WARN("[%.4f][InitializationScene::Update] Server rejected our client!", APP_RUN_TIME);
				LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] Server rejected our client!", APP_RUN_TIME);
				m_application->Exit();
			break;
			}

			case net::EMessageId::NET_MSG_CLIENT_ACCEPT:
			{
				LOG_GAME_WARN("[%.4f][InitializationScene::Update] Server accepted our client!", APP_RUN_TIME);
				LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] Server accepted our client!", APP_RUN_TIME);
				break;
			}

			case net::EMessageId::NET_MSG_USER_DATA:
			{
				LOG_GAME_WARN("[%.4f][InitializationScene::Update] User Data received!", APP_RUN_TIME);
				LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] User Data received!", APP_RUN_TIME);

				READ_MESSAGE(in, packet);
				m_application->m_localClientDesc = new net::SClientDescription();
				m_application->m_localClientDesc->Deserialize(in, true);

				m_initializationComplete = true;

				// Store the MasterServer address for messaging from NetManager purposes.
				CNetCommMngr::InitializeMasterServerConnection(packet->systemAddress);


				// Request the Ability and Status Effects Data.
				CREATE_MESSAGE(out, net::EMessageId::NET_MSG_REQUEST_ABILITY_AND_STATUS_EFFECTS_DATA);
				m_application->Send(out, packet->systemAddress);

			break;
			}

			case DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED:
			{
				m_application->Exit();
				break;
			}


			default:
			{
				LOG_GAME_WARN("[%.4f][InitializationScene::Update] Unrecognized Message! Id: %s", APP_RUN_TIME, net::MessageIDTypeToString((RakNet::MessageID)packet->data[0]).C_String());
				LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] Unrecognized Message! Id: {}", APP_RUN_TIME, net::MessageIDTypeToString((RakNet::MessageID)packet->data[0]).C_String());
			break;
			}
			}


			LOG_GAME_WARN("[%.4f][InitializationScene::Update] Deallocate Package! Id: %s", APP_RUN_TIME,  net::MessageIDTypeToString((RakNet::MessageID)packet->data[0]).C_String());
			LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] Deallocate Package! Id: {}", APP_RUN_TIME, net::MessageIDTypeToString((RakNet::MessageID)packet->data[0]).C_String());
			m_application->DeallocateMessage(packet);
		}
	}
	if (m_application->Connected() && m_initializationComplete && (initialization_scene->m_statusEffectsDataDownloadComplete && initialization_scene->m_abilityDataDownloadComplete) == false)
	{
		auto packet = m_application->PopNextMessage();
		if (packet)
		{
			RakNet::MessageID id = (RakNet::MessageID)packet->data[0];

			switch(id)
			{
				// Retrieve Data and store it.
			case net::EMessageId::NET_MSG_ABILITY_DATA:
			{
				READ_MESSAGE(stream, packet);

				// We received one (=1) ability data definition.

				net::SAbilityData ability;
				ability.Deserialize(stream, true);

				LOG_GAME_WARN("[%.4f][InitializationScene::Update] EntityAbility: \"%s\"", APP_RUN_TIME, ability.m_abilityName.C_String());

				EntityAbilitiesDataMap::get()->Data(ability);
				break;
			}

			case net::EMessageId::NET_MSG_STATUS_EFFECTS_DATA:
			{
				READ_MESSAGE(stream, packet);

				// We received one (=1) status effect data definition.

				net::SStatusEffectData status_effect;
				status_effect.Deserialize(stream, true);

				LOG_GAME_WARN("[%.4f][InitializationScene::Update] EntityStatusEffect: \"%s\"", APP_RUN_TIME, status_effect.m_effectName.C_String());

				EntityStatusEffectsDataMap::get()->Data(status_effect);
				break;
			}

			case net::EMessageId::NET_MSG_ABILITY_AND_STATUS_EFFECT_DATA_COMPLETE:
			{
				// Ability and Status Effect Data transfer complete.
				initialization_scene->m_abilityDataDownloadComplete = true;
				initialization_scene->m_statusEffectsDataDownloadComplete = true;
				break;
			}
			}

			m_application->DeallocateMessage(packet);
		}
	}

	if (m_initializationComplete)
	{
		LOG_GAME_INFO("[%.4f][InitializationScene] Initialization complete", APP_RUN_TIME);
		LOG_DBG_INFO("[{:.4f}][InitializationScene] Initialization complete", APP_RUN_TIME);
	}
	if (initialization_scene->m_abilityDataDownloadComplete && initialization_scene->m_statusEffectsDataDownloadComplete)
	{
		LOG_GAME_INFO("[%.4f][InitializationScene] Ability and Status Effects Data download complete", APP_RUN_TIME);
		LOG_DBG_INFO("[{:.4f}][InitializationScene] Ability and Status Effects Data download complete", APP_RUN_TIME);
	}

	if (m_initializationComplete && initialization_scene->m_statusEffectsDataDownloadComplete && initialization_scene->m_abilityDataDownloadComplete)
	{
		m_stateMachine->Transit("SplashScreen");
	}
}
void cherrysoda::SplashSceenScene::SceneImpl::Update()
{
	if (!m_initializationComplete) return;

	// Base update.
	cherrysoda::Scene::Update();

	LOG_DBG_WARN("[{:.4f}][SplashSceenScene::Update] SplashScreenScene transitions directly to GameScene...", APP_RUN_TIME);
	LOG_DBG_ERROR("[{:.4f}][SplashSceenScene::Update] GameScene not implemented!", APP_RUN_TIME);
	LOG_GAME_WARN("[%.4f][SplashSceenScene::Update] SplashScreenScene transitions directly to GameScene...", APP_RUN_TIME);
	LOG_GAME_ERROR("[%.4f][SplashSceenScene::Update] GameScene not implemented!", APP_RUN_TIME);


	m_stateMachine->Transit("GameScene");
}
void cherrysoda::SplashSceenScene::SceneImpl::Begin()
{
	LOG_GAME_INFO("[%.4f][SplashSceenScene::Begin] ... ", APP_RUN_TIME);

	using namespace cherrysoda;

	// Initialize rendering.
	cherrysoda::Graphics::SetPointTextureSampling();
	auto renderer = new cherrysoda::EverythingRenderer();
	auto camera = renderer->GetCamera();
	camera->Position(cherrysoda::Math::Vec3(0.0f, 0.0f, 200.0f));
	renderer->SetEffect(cherrysoda::Graphics::GetEmbeddedEffect("sprite")); // Make a sprite renderer.
	renderer->KeepCameraCenterOrigin(true);
	camera->UseOrthoProjection(true);
	camera->CenterOrigin();
	Add(renderer);

	m_initializationComplete = true;
}
void cherrysoda::SplashSceenScene::SceneImpl::End()
{
	LOG_GAME_INFO("[%.4f][SplashSceenScene::End] ... ", APP_RUN_TIME);
}


void cherrysoda::DebugGameScene::SceneImpl::Update()
{
	//LOG_GAME_INFO("[DebugGameScene] Update");

	// Base update.
	cherrysoda::Scene::Update();
}
void cherrysoda::DebugGameScene::SceneImpl::End()
{
	LOG_GAME_INFO("[%.4f][DebugGameScene::End] ... ", APP_RUN_TIME);
}
void cherrysoda::DebugGameScene::SceneImpl::Begin()
{
	LOG_GAME_INFO("[%.4f][DebugGameScene::Begin] ... ", APP_RUN_TIME);
	GUI::DisableInternalConsole();

	
	// Initialize rendering.
	cherrysoda::Graphics::SetPointTextureSampling();
	auto renderer = new cherrysoda::CustomRenderer();
	auto camera = renderer->GetCamera();
	camera->Position(cherrysoda::Math::Vec3(0.0f, 0.0f, 200.0f));
	renderer->SetEffect(cherrysoda::Graphics::GetEmbeddedEffect("sprite")); // Make a sprite renderer.
	renderer->KeepCameraCenterOrigin(false);
	camera->UseOrthoProjection(true);
	camera->CenterOrigin();
	Add(renderer);
	

	auto factory = Factory::get();
	
	// Maptiles around Town
	auto entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(0.0f, 0.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(256.0f, 0.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(0.0f, 256.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(256.0f, 256.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(-256.0f, 0.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(0.0f, -256.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(-256.0f, -256.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(256.0f, -256.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	entity = factory->Begin(this)
		.Add(new  Sprite("assets/MaptileAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "snow");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(-256.0f, 256.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(2);


	// Town
	entity = factory->Begin(this)
		.Add(new  Sprite("assets/BuildingAtlas.json"))
		.Add(new  SelectableBuilding(1))
		.Add(new  CollidableComponent(true, false, true))
		.Add(new  Building("Townhall", 1, 500, 1, 1, 250.0f, 250.0f))
		.Add(new EntityAbilityMap())
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "Human_Townhall_III_Winter");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(0.0f, 0.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(1);


	// Unit
	entity = factory->Begin(this)
		.Add(new  Sprite("assets/UnitAtlas.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "DarkElf_Knight_Ebony");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(-256.0f, 0.0f));
	entity->Get< Sprite >()->CenterOrigin();
	//entity->Get< Sprite >()->Scale2D({ 0.75f, 0.75f });
	entity->Depth(0);

	// Unit
	entity = factory->Begin(this)
		.Add(new  Sprite("assets/UnitAtlas.json"))
		.Add(new Unit("Steel Swordman", 1, 50, 200, 25, 50, 0, 0, 0, 0))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "Human_Swordman_Steel");
	entity->Get< Sprite >()->Play("Idle");
	entity->Position2D(Math::Vec2(0.0f, 0.0f));
	entity->Get< Sprite >()->Position2D(Math::Vec2(0.0f, 0.0f));
	entity->Get< Sprite >()->CenterOrigin();
	//entity->Get< Sprite >()->Scale2D({ 0.75f, 0.75f });
	entity->Depth(0);

	entity = factory->Begin(this)
		.Add(new  SelectableUnit(1))
		.Add(new  Sprite("assets/UnitAtlas.json"))
		.Add(new  CollidableComponent(true, false, true))
		.Add(new  Unit("HighElf_Swordman_Malachite", 1, 20, 1, 1, 5, 0, 0, 0, 0))
		.End();
	
	// Create custom Animation adding them "Frame by Frame".
	entity->Get< Sprite >()->Add("first", "HighElf_Swordman_Malachite", 0.16f,
									 cherrysoda::Chooser<StringID>("second", 1.0f));

	entity->Get< Sprite >()->Add("second", "HighElf_Swordman_Copper", 0.16f,
									 cherrysoda::Chooser<StringID>("third", 1.0f));

	entity->Get< Sprite >()->Add("third", "HighElf_Swordman_Adamantine", 0.16f,
									 cherrysoda::Chooser<StringID>("fourth", 1.0f));

	auto chooser = cherrysoda::Chooser<StringID>("first", 0.5f);
	chooser.Add("second", 0.5f);

	entity->Get< Sprite >()->Add("fourth", "HighElf_Swordman_Adamantine", 0.16f,
								chooser);


	entity->Get< Sprite >()->Play("first");
	entity->Position2D(Math::Vec2(0.0f, 0.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(1);


	// Camera control
	entity = factory->Begin(this)
		.Add(new CameraController(camera))
		.End();

}

using GameApp = App;
CHERRYSODA_DEFAULT_MAIN(1280, 720)