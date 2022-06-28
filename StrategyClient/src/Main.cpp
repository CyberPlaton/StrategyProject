#include "Main.h"

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
uint64_t App::GetVersion()
{
	return 1000;
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


			LOG_DBG_INFO("[{:.4f}] Loading Scene: {}:", APP_RUN_TIME(), name);
			LOG_GAME_INFO("[%.4f] Loading Scene: \"%s\"", APP_RUN_TIME(), name);

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
				LOG_DBG_CRITICAL("[{:.4f}] Could not load unknown scene type: {}", APP_RUN_TIME(), name);
				LOG_FILE_CRITICAL("[{:.4f}] Could not load unknown scene type: {}", APP_RUN_TIME(), name);
				return false;
			}


			// Set as starting scene if declared.
			if (initial)
			{
				app->m_stateMachine->SetInitialState(new_scene);
				app->SetScene(new_scene->AsScene());
				LOG_DBG_INFO("\t ... as Initial");
				LOG_GAME_INFO("\t ... as Initial");
			}


			auto transit = scene->FirstChildElement("Transit");
			while (transit)
			{
				auto transit_to = transit->GetText();

				// Add Transit.
				LOG_DBG_INFO("\tAdd Transition To: {}", transit_to);
				LOG_GAME_INFO("\tAdd Transition To: \"%s\"", transit_to);

				new_scene->AddTransition(transit_to);

				transit = transit->NextSiblingElement("Transit");
			}

			scene = scene->NextSiblingElement("Scene");
		}

		return true;
	}

	return false;
}

void NetCommMngr::UpdateNetGameobject(net::SGameobject* object)
{
	printf("NetCommMngr::UpdateNetGameobject\n");

	/*
	olc::net::message< net::Message > out;
	out.header.id = net::Message::NET_MSG_GAMEOBJECT_UPDATE;
	out << object;

	g_App->Send(out);
	*/
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
		LOG_DBG_CRITICAL("[{:.4f}] Logger not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Logger not initialized!", APP_RUN_TIME());
		base::Exit(); 
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Logger initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Logger initialized...", APP_RUN_TIME());


	if (!InitializeLocalization())
	{
		LOG_DBG_CRITICAL("[{:.4f}] Localization not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Localization not initialized!", APP_RUN_TIME());
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Localization initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Localization initialized...", APP_RUN_TIME());


	// Initialize the Platform Client.
	if (!InitializePlatformClient())
	{
		LOG_DBG_CRITICAL("[{:.4f}] Platform client not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Platform client not initialized!", APP_RUN_TIME());
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Platform client initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Platform client initialized...", APP_RUN_TIME());


	// Initialize the GamerService.
	if (!InitializeGamerService())
	{
		LOG_DBG_CRITICAL("[{:.4f}] Gamer service not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Gamer service not initialized!", APP_RUN_TIME());
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Gamer service initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Gamer service initialized...", APP_RUN_TIME());

	if (!InitializeMasterConnection())
	{
		LOG_DBG_CRITICAL("[{:.4f}] Master connection not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Master connection not initialized!", APP_RUN_TIME());
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Master connection initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Master connection initialized...", APP_RUN_TIME());


	if (!NetCommMngr::Initialize(this))
	{
		LOG_DBG_CRITICAL("[{:.4f}] Net communication manager not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Net communication manager not initialized!", APP_RUN_TIME());
		base::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Net communication manager initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Net communication manager initialized...", APP_RUN_TIME());
	LOG_DBG_INFO("[{:.4f}] SUBSYSTEMS SUCCESSFULLY INITIALIZED!", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] SUBSYSTEMS SUCCESSFULLY INITIALIZED!", APP_RUN_TIME());


	// Everything went OK. Startup the engine!
	base::ExitOnEscapeKeypress(false);
	base::Initialize();

	// Load Game Scenes!
	if (!cherrysoda::SceneGraphFactory::LoadSceneGraph("assets/SceneGraph.xml", this))
	{
		LOG_DBG_CRITICAL("[{:.4f}] Failed loading SceneGraph!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Failed loading SceneGraph!", APP_RUN_TIME());
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
	LOG_DBG_INFO("[{:.4f}] Terminating Platform client...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Terminating Platform client...", APP_RUN_TIME());
	TerminatePlatformClient();

	// DELETE GAMERSERVICE
	LOG_DBG_INFO("[{:.4f}] Terminating Gamer service...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Terminating Gamer service...", APP_RUN_TIME());
	TerminateGamerService();

	// DELTET MASTERSERVER CONNECTION
	LOG_DBG_INFO("[{:.4f}] Terminating Master connection...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Terminating Master connection...", APP_RUN_TIME());
	TerminateMasterConnection();

	// DELETE STEAM
	LOG_DBG_INFO("[{:.4f}] Terminating SteamAPI...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Terminating SteamAPI...", APP_RUN_TIME());
	SteamAPI_Shutdown();

	// DELETE LOCALIZATION
	LOG_DBG_INFO("[{:.4f}] Terminating Localization...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Terminating Localization...", APP_RUN_TIME());
	TerminateLocalization();

	// DELETE LOG
	LOG_DBG_INFO("[{:.4f}] Terminating Logger...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Terminating Logger...", APP_RUN_TIME());
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
		LOG_GAME_INFO("[App::InitializeMasterConnection] Connecting...");

#ifndef OFFLINE_GAME_CLIENT_TEST
		if (!ClientInterface::Initialize(MASTER_SERVER_PORT, MASTER_SERVER_IP))
		{
			LOG_GAME_CRITICAL("[InitializationScene::InitializeMasterConnection] Could not initialize ClientInterface!");
			LOG_DBG_CRITICAL("[InitializationScene::InitializeMasterConnection] Could not initialize ClientInterface!");
			return false;
		}

		LOG_GAME_INFO("[App::InitializeMasterConnection] ClientInterface initialized!"); 

		Timer timeout_timer;
		timeout_timer.StartTimer();

		while (!ClientInterface::Connected() && timeout_timer.SecondsElapsed() < CONNECTION_TIMEOUT_TIMER_SECONDS)
		{
			ClientInterface::Update();
		}

		return ClientInterface::Connected();
#else
		LOG_GAME_WARN("[%.4f][InitializeMasterConnection] Connection skipped due to offline test!", APP_RUN_TIME());
		LOG_DBG_WARN("[{:.4f}][InitializeMasterConnection] Connection skipped due to offline test!", APP_RUN_TIME());
		return true;
#endif
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
	LOG_GAME_INFO("[InitializationScene] Begin");
}
void cherrysoda::InitializationScene::SceneImpl::End()
{
	LOG_GAME_INFO("[InitializationScene] End");
}
void cherrysoda::InitializationScene::SceneImpl::Update()
{
	static bool show_demo;
	ImGui::ShowDemoWindow(&show_demo);

#if OFFLINE_GAME_CLIENT_TEST
	LOG_GAME_WARN("[InitializationScene::Update] Debug: Transit to DebugGameScene");
	LOG_DBG_WARN("[InitializationScene::Update] Debug: Transit to DebugGameScene");
	m_stateMachine->Transit("DebugGame");
	return;
#endif

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
				LOG_GAME_WARN("[InitializationScene::Update] Validation Data requested!");
				LOG_DBG_WARN("[InitializationScene::Update] Validation Data requested!");

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

				CREATE_MESSAGE(net::EMessageId::NET_MSG_USER_VALIDATION_DATA);
				clientDesc.Serialize(stream);
				m_application->Send(stream, packet->systemAddress);
				break;
			}
			
			case net::EMessageId::NET_MSG_CLIENT_REJECT:
			{
				LOG_GAME_WARN("[InitializationScene::Update] Server rejected our client!");
				LOG_DBG_WARN("[InitializationScene::Update] Server rejected our client!");
				m_application->Exit();
			break;
			}

			case net::EMessageId::NET_MSG_CLIENT_ACCEPT:
			{
				LOG_GAME_WARN("[InitializationScene::Update] Server accepted our client!");
				LOG_DBG_WARN("[InitializationScene::Update] Server accepted our client!");
				break;
			}

			case net::EMessageId::NET_MSG_USER_DATA:
			{
				LOG_GAME_WARN("[InitializationScene::Update] User Data received!");
				LOG_DBG_WARN("[InitializationScene::Update] User Data received!");

				READ_MESSAGE(packet);
				m_application->m_localClientDesc = new net::SClientDescription();
				m_application->m_localClientDesc->Deserialize(stream, true);

				m_initializationComplete = true;
			break;
			}

			case DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED:
			{
				m_application->Exit();
			}

			default:
			{
				LOG_GAME_WARN("[%.4f][InitializationScene::Update] Unrecognized Message! Id: %s", APP_RUN_TIME(), net::MessageIDTypeToString((RakNet::MessageID)packet->data[0]).C_String());
				LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] Unrecognized Message! Id: {}", APP_RUN_TIME(), net::MessageIDTypeToString((RakNet::MessageID)packet->data[0]).C_String());
			break;
			}
			}


			LOG_GAME_WARN("[%.4f][InitializationScene::Update] Deallocate Package! Id: %s", APP_RUN_TIME(),  net::MessageIDTypeToString((RakNet::MessageID)packet->data[0]).C_String());
			LOG_DBG_WARN("[{:.4f}][InitializationScene::Update] Deallocate Package! Id: {}", APP_RUN_TIME(), net::MessageIDTypeToString((RakNet::MessageID)packet->data[0]).C_String());
			m_application->DeallocateMessage(packet);
		}
	}
	if (m_initializationComplete)
	{
		LOG_GAME_INFO("[%.4f][InitializationScene] Initialization complete", APP_RUN_TIME());
		LOG_DBG_WARN("[{:.4f}][InitializationScene] Initialization complete", APP_RUN_TIME());
		m_stateMachine->Transit("SplashScreen");
	}
}
void cherrysoda::SplashSceenScene::SceneImpl::Update()
{
	if (!m_initializationComplete) return;

	// Base update.
	cherrysoda::Scene::Update();

	LOG_DBG_WARN("[{:.4f}] SplashScreenScene transitions directly to GameScene...", APP_RUN_TIME());
	LOG_DBG_ERROR("[{:.4f}] GameScene not implemented!", APP_RUN_TIME());
	LOG_GAME_WARN("[%.4f] SplashScreenScene transitions directly to GameScene...", APP_RUN_TIME());
	LOG_GAME_ERROR("[%.4f] GameScene not implemented!", APP_RUN_TIME());

	m_stateMachine->Transit("GameScene");
}
void cherrysoda::SplashSceenScene::SceneImpl::Begin()
{
	LOG_GAME_INFO("[SplashSceenScene] Begin");

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
	LOG_GAME_INFO("[SplashSceenScene] End");
}


void cherrysoda::DebugGameScene::SceneImpl::Update()
{
	//LOG_GAME_INFO("[DebugGameScene] Update");

	// Base update.
	cherrysoda::Scene::Update();
}
void cherrysoda::DebugGameScene::SceneImpl::End()
{
	LOG_GAME_INFO("[DebugGameScene] End");
}
void cherrysoda::DebugGameScene::SceneImpl::Begin()
{
	LOG_GAME_INFO("[DebugGameScene] Begin");

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
		.Add(new ScriptComponent("assets/testing_script.lua"))
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
	entity->Position2D(Math::Vec2(256.0f, 0.0f));
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
	entity->Position2D(Math::Vec2(256.0f, -256.0f));
	entity->Get< Sprite >()->CenterOrigin();
	entity->Depth(0);

	// Camera control
	entity = factory->Begin(this)
		.Add(new CameraController(camera))
		.End();

}


struct Test
{
	int h;
	int w;
	std::string name;
};

Test oTest;
bool bTest = false;


// SCRIPTING
// BIND ANY SCRIPTRING RELEVANT FUNCTIONS AND DATASTRUCTURES TO LUA
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




using GameApp = App;
CHERRYSODA_DEFAULT_MAIN(1280, 720)