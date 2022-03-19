#include "Main.h"

bool cherrysoda::SceneGraphFactory::LoadSceneGraph(const cherrysoda::String& filename, App* app)
{
	using namespace cherrysoda;
	using namespace cherrysoda::xml;

	XMLUtil xml;
	XMLDocument doc;
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

void NetCommMngr::UpdateNetGameobject(net::NetGameobject& object)
{
	printf("NetCommMngr::UpdateNetGameobject\n");

	olc::net::message< net::Message > out;
	out.header.id = net::Message::NET_MSG_GAMEOBJECT_UPDATE;
	out << object;

	g_App->Send(out);
}
App::App() : base(), 
m_DefaultTexture(nullptr), m_Image(nullptr)
{
	SetTitle(TITLE(CLIENT_TITLE_STRING, CLIENT_MAJOR_VERSION, CLIENT_MINOR_VERSION, CLIENT_REVIEW_VERSION));
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

	// LOGGER UPDATE
	cherrysoda::Logger::Update();
}

void App::Initialize()
{
	if (!InitializeLogger(true, false, 256))
	{
		LOG_DBG_CRITICAL("[{:.4f}] Logger not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Logger not initialized!", APP_RUN_TIME());
		App::Exit(); 
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Logger initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Logger initialized...", APP_RUN_TIME());


	if (!InitializeLocalization())
	{
		LOG_DBG_CRITICAL("[{:.4f}] Localization not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Localization not initialized!", APP_RUN_TIME());
		App::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Localization initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Localization initialized...", APP_RUN_TIME());


	// Initialize the Platform Client.
	if (!InitializePlatformClient())
	{
		LOG_DBG_CRITICAL("[{:.4f}] Platform client not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Platform client not initialized!", APP_RUN_TIME());
		App::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Platform client initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Platform client initialized...", APP_RUN_TIME());


	// Initialize the GamerService.
	if (!InitializeGamerService())
	{
		LOG_DBG_CRITICAL("[{:.4f}] Gamer service not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Gamer service not initialized!", APP_RUN_TIME());
		App::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Gamer service initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Gamer service initialized...", APP_RUN_TIME());

	if (!InitializeMasterConnection())
	{
		LOG_DBG_CRITICAL("[{:.4f}] Master connection not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Master connection not initialized!", APP_RUN_TIME());
		App::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Master connection initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Master connection initialized...", APP_RUN_TIME());


	if (!NetCommMngr::Initialize(this))
	{
		LOG_DBG_CRITICAL("[{:.4f}] Net communication manager not initialized!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Net communication manager not initialized!", APP_RUN_TIME());
		App::Exit();
		return;
	}
	LOG_DBG_INFO("[{:.4f}] Net communication manager initialized...", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] Net communication manager initialized...", APP_RUN_TIME());
	LOG_DBG_INFO("[{:.4f}] SUBSYSTEMS SUCCESSFULLY INITIALIZED!", APP_RUN_TIME());
	LOG_FILE_INFO("[{:.4f}] SUBSYSTEMS SUCCESSFULLY INITIALIZED!", APP_RUN_TIME());



	// Everything went OK. Startup the engine!
	base::Initialize();


	// Load Game Scenes!
	if (!cherrysoda::SceneGraphFactory::LoadSceneGraph("assets/SceneGraph.xml", this))
	{
		LOG_DBG_CRITICAL("[{:.4f}] Failed loading SceneGraph!", APP_RUN_TIME());
		LOG_FILE_CRITICAL("[{:.4f}] Failed loading SceneGraph!", APP_RUN_TIME());
		App::Exit();
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
		LOG_GAME_INFO("[App::InitializeMasterConnection] Try connect to Master server");

		// Establish connection with MasterServer.
		if (Connect(MASTER_SERVER_IP, MASTER_SERVER_PORT))
		{
			// Whether we are truly connected still needs to be proofed.
			return true;
		}
	}
	return false;
}
void App::TerminateMasterConnection()
{
	Disconnect();
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

#ifdef DEBUG
	LOG_GAME_WARN("[InitializationScene] Debug: Transit to DebugGameScene");
	LOG_DBG_WARN("[InitializationScene] Debug: Transit to DebugGameScene");
	m_stateMachine->Transit("DebugGame");
#endif

	if (m_application->IsConnected() && m_initializationComplete == false)
	{
		if (!m_application->Incoming().empty())
		{
			// Get the Message.
			auto msg = m_application->Incoming().pop_front().msg;


			// Read the Message and react to it.
			// In the Init Scene we only expect a few kinds of Messages.
			switch (msg.header.id)
			{
			case net::NET_MSG_REQUEST_USER_VALIDATION_DATA:
			{
				LOG_GAME_INFO("[InitializationScene] User Data for validation requested");

				net::UserDesc user_desc;
				net::ClientAppDesc app_desc;

				// App Desc.
				app_desc.m_majorVersion = CLIENT_MAJOR_VERSION;
				app_desc.m_minorVersion = CLIENT_MINOR_VERSION;
				app_desc.m_reviewVersion = CLIENT_REVIEW_VERSION;

				// User Desc.
				user_desc.m_steamName = PlatformClient::get()->GetClientPlatformName();
				user_desc.m_steamId = PlatformClient::get()->GetClientPlatformID();
				switch (PlatformClient::get()->GetClientPlatform())
				{
				case net::EUserPlatform::UP_STEAM:
					user_desc.m_userPlatform = net::EUserPlatform::UP_STEAM;
					break;
				case net::EUserPlatform::UP_SWITCH:
					user_desc.m_userPlatform = net::EUserPlatform::UP_SWITCH;
					break;
				default:
					m_application->Exit();
					break;
				}

				EncryptMessage(app_desc);
				EncryptMessage(user_desc);

				olc::net::message< net::Message > message;
				message.header.id = net::Message::NET_MSG_USER_VALIDATION_DATA;
				message << app_desc;
				message << user_desc;

				m_application->Send(message);
			}
			break;



			// We were rejected.
			case net::NET_MSG_CLIENT_REJECT:
				LOG_GAME_INFO("[InitializationScene] Rejected by Master server");
				m_application->Exit();
				break;


			// We were accepted and received our user data from Database.
			case net::NET_MSG_USER_DATA:
			{
				LOG_GAME_INFO("[InitializationScene] Received user data");

				net::UserDesc desc;
				msg >> desc;

				DecryptMessage(desc);

				// Store user data in the client for usage.
				// Take ownership over the Object.
				m_application->m_localUserDesc = new net::UserDesc(desc);

				m_initializationComplete = true;
			}
			break;



			default:
				m_application->Exit();
			}
		}
	}
	else
	{
		m_application->Exit();
	}


	if (m_initializationComplete)
	{
		LOG_GAME_INFO("[InitializationScene] Initialization complete");
		m_stateMachine->Transit("SplashScreen");
	}
}
void cherrysoda::SplashSceenScene::SceneImpl::Update()
{
	if (!m_initializationComplete) return;

	// Base update.
	cherrysoda::Scene::Update();

	LOG_DBG_WARN("[{:.4f}] SplashScreenScene transitions directly to GameScene...", APP_RUN_TIME());

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

	auto camera = FirstRenderer()->GetCamera();

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
	auto renderer = new cherrysoda::EverythingRenderer();
	auto camera = renderer->GetCamera();
	camera->Position(cherrysoda::Math::Vec3(0.0f, 0.0f, 200.0f));
	renderer->SetEffect(cherrysoda::Graphics::GetEmbeddedEffect("sprite")); // Make a sprite renderer.
	renderer->KeepCameraCenterOrigin(false);
	camera->UseOrthoProjection(true);
	camera->CenterOrigin();
	Add(renderer);


	
	auto factory = Factory::get();
	

	auto entity = factory->Begin(this)
		.Add(new  Sprite("assets/Textures.json"))
		.End();
	entity->Get< Sprite >()->AddLoop("Idle", "HU_Townhall_III");
	entity->Get< Sprite >()->Play("Idle");
	entity->Get< Sprite >()->Position2D(Math::Vec2(100.0f, 100.0f));
	entity->Get< Sprite >()->CenterOrigin();


	entity = factory->Begin(this)
		.Add(new CameraController(camera))
		.End();
}





using GameApp = App;
CHERRYSODA_DEFAULT_MAIN