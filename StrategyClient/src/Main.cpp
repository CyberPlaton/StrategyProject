#include "Main.h"

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

	/*
	// PHYSICS UPDATE
	float time_step = 0.16f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;
	m_World->Step(time_step, velocityIterations, positionIterations);
	

	ImGui::Begin("Test");
	if (ImGui::Button("Close"))
	{
		App::Exit();
	}
	if (ImGui::Button("Toggle Fullscreen"))
	{
		App::ToggleFullscreen();
	}
	ImGui::End();


	size_t entities_count = 0;
	for (auto e = GetScene()->Entities()->begin();
		e != GetScene()->Entities()->end();
		e++)
	{
		entities_count++;
	}

	ImGui::Begin("Perf");
	ImGui::Text("FPS \"%d\"", FPS());
	ImGui::Text("Entites \"%d\"", entities_count);

	ImGui::End();

	auto padone = cherrysoda::MInput::GamePads(0);
	auto padtwo = cherrysoda::MInput::GamePads(1);

	if (padone->Attached())
	{
		cout << "Pad One Attached" << endl;
	}
	else
	{
		padone->UpdateNull();
		cout << "Pad One NOT Attached" << endl;
	}
	if (padtwo->Attached())
	{
		cout << "Pad Two Attached" << endl;
	}
	else
	{
		padtwo->UpdateNull();
		cout << "Pad Two NOT Attached" << endl;
	}

	if (padone->Pressed(cherrysoda::Buttons::A))
	{
		padone->Rumble(0.4f, 5.0f);

		// Draw Line
		// Line Initialized and Rendered constantly.
		cherrysoda::Line::CreateLine2D(GetScene(), cherrysoda::Math::Vec2(-rand() % 1000, rand() % 1000),
			cherrysoda::Math::Vec2(rand() % 1000, -rand() % 1000), cherrysoda::Color::Blue, 1.0f);
	}
	if (padtwo->Pressed(cherrysoda::Buttons::A))
	{
		padtwo->Rumble(0.4f, 5.0f);

		// Draw Line
		// Line Initialized and Rendered constantly.
		cherrysoda::Line::CreateLine2D(GetScene(), cherrysoda::Math::Vec2(-rand() % 1000, rand() % 1000),
			cherrysoda::Math::Vec2(rand() % 1000, -rand() % 1000), cherrysoda::Color::DarkRed, 1.0f);
	}


	// Try and find Walter
	auto list = m_scene->GetEntitiesByTagMask(m_entityNameTagMap["Walter"]);
	if (!list.empty())
	{
		printf("Size of List with name \"Walter\" %d \n", list.size());
	}
	*/
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

	// Create a Statemachine for scene transitions.
	m_stateMachine = new StateMachine(this);

	// Create a scene for first connection to masterserver and loging in.
	auto init_scene = new InitializationScene("Initialization", m_stateMachine, this);
	init_scene->AddTransition("SplashScreen");
	m_stateMachine->SetInitialState(init_scene);
	auto scene = init_scene->AsScene();
	SetScene(scene);

	// Load other Scenes.
	// It would be a good Idea not to load all scenes in the start,
	// but rather load the needed one async in another scene.
	auto splash_screen = new SplashSceenScene("SplashScreen", m_stateMachine, this);
	splash_screen->AddTransition("MainMenu");


	m_font = new cherrysoda::PixelFont("PixelFont");
	auto atlas = cherrysoda::Atlas::FromAtlas("assets/font/atlases.json");
	m_font->AddFontSize("assets/font/font_json.json", atlas);

	// Check whether connection was initialized and we are connected.
	if (!IsConnected())
	{
		App::Exit();
	}


	/*
	cherrysoda::Graphics::SetPointTextureSampling();

	// Create a scene.
	auto scene = new cherrysoda::Scene();

	// Create a renderer.
	auto renderer = new cherrysoda::EverythingRenderer();
	auto camera = renderer->GetCamera();
	camera->Position(cherrysoda::Math::Vec3(0.0f, 0.0f, 200.0f));
	renderer->SetEffect(cherrysoda::Graphics::GetEmbeddedEffect("sprite")); // Make a sprite renderer.
	renderer->KeepCameraCenterOrigin(true);
	camera->UseOrthoProjection(true);
	camera->CenterOrigin();


	// Add renderer to scene to render it...
	scene->Add(renderer);

	// .. and set it as running scene.
	SetScene(scene);

	// Add entity with sprite to scene.
	cherrysoda::String json_path = Platform::GetDefaultAssetsPath() + "/maptiles.json";
	auto sprite = new cherrysoda::Sprite(json_path);
	sprite->Position2D(cherrysoda::Math::Vec2(160.02f, 250.0f));
	sprite->AddLoop("title", "forest_jungle_normal"); // For the whole animation define "name"
	//sprite->AddLoop("title", "cherrysoda00"); // For the just a sprite in "images" from the animation sprites
	sprite->Play("title");
	sprite->CenterOrigin();
	sprite->Justify(cherrysoda::Math::Vec2(0.5f));

	auto entity = new cherrysoda::Entity();
	entity->Add(sprite);
	entity->AddTag(++g_iEntityID);


	scene->Add(entity);


	// INITIALIZE BOX2D
	b2Vec2 gravity_vec(0.0f, 0.0f);
	m_World = new b2World(gravity_vec);

	b2BodyDef ground_body_def;
	ground_body_def.position.Set(5.0f, 5.0f);

	b2Body* ground_body = m_World->CreateBody(&ground_body_def);

	b2PolygonShape ground_box;
	ground_box.SetAsBox(10.0f, 3.0f);

	ground_body->CreateFixture(&ground_box, 0.5f);

	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.linearDamping = 1.0f;
	body_def.angularDamping = 1.0f;
	body_def.position.Set(5.0f, 4.0f);
	b2Body* body = m_World->CreateBody(&body_def);

	b2PolygonShape dynamic_box;
	dynamic_box.SetAsBox(1.0f, 1.0f);
	b2FixtureDef fixture_def;
	fixture_def.shape = &dynamic_box;
	fixture_def.density = 1.0f;
	fixture_def.friction = 0.7f;

	body->CreateFixture(&fixture_def);

	float time_step = 0.16f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;



	// INITIALIZE AUDIO
	cherrysoda::Audio::Initialize();
	cherrysoda::Audio::LoadFile("main_theme", "assets/atmosphere_2.ogg");
	cherrysoda::Audio::MasterVolume(0);

	cherrysoda::Audio::EventInstance& inst = cherrysoda::Audio::Loop("main_theme");
	if (inst.IsValid())
	{
		cherrysoda::Audio::Stop(inst);
	}



	// TESTING XML
	cherrysoda::XML::XMLDocument doc;
	if (cherrysoda::XMLUtil::ReadXMLFile(doc, "assets/sheet_data.xml"))
	{
		printf_s("Reading XML sucessful!");
		doc.Clear();
	}





	// TESTING EVENT SYSTEM
	// Create Event listeners, which wait for certain events.
	cherrysoda::EventListener* event_listener = new cherrysoda::EventListener("Evnt_Application_Start");
	cherrysoda::EventListener* event_listener_2 = new cherrysoda::EventListener("Evnt_Application_Update");
	
	// Create Scene wide event system.
	cherrysoda::EventSystem* event_system = new cherrysoda::EventSystem();

	// Create entities for scene.
	auto event_listener_entity = new cherrysoda::Entity();
	event_listener_entity->Add(event_listener);
	event_listener_entity->AddTag(++g_iEntityID);
	size_t event_system_id = g_iEntityID;

	auto event_listener_entity_2 = new cherrysoda::Entity();
	event_listener_entity_2->Add(event_listener_2);
	event_listener_entity_2->AddTag(++g_iEntityID);

	auto event_system_entity = new cherrysoda::Entity();
	event_system_entity->Add(event_system);
	event_system_entity->AddTag(++g_iEntityID);


	// Add listeners to the event system.
	event_system_entity->Get<cherrysoda::EventSystem>()->Add(event_listener, event_listener->EventType());
	event_system_entity->Get<cherrysoda::EventSystem>()->Add(event_listener_2, event_listener_2->EventType());


	// Create an emitter.
	cherrysoda::EventEmitter* emitter = new cherrysoda::EventEmitter(TEST_EmitEvent, event_system_id);
	auto event_emitter_entity = new cherrysoda::Entity();
	event_emitter_entity->Add(emitter);
	event_emitter_entity->AddTag(++g_iEntityID);


	// Add event entities to scene.
	scene->Add(event_listener_entity);
	scene->Add(event_listener_entity_2);
	scene->Add(event_system_entity);
	scene->Add(event_emitter_entity);


	// Sound Entity
	auto sound_entity = new cherrysoda::Entity();
	auto sound = new cherrysoda::Sound("", "");
	sound_entity->Add(sound);
	sound_entity->AddTag(++g_iEntityID);



	auto particle_system = new cherrysoda::ParticleSystem(0, 20000);
	auto particle_type = new cherrysoda::ParticleType();
	particle_type->m_color = cherrysoda::Color::Yellow;
	particle_type->m_color2 = cherrysoda::Color::Red;
	particle_type->m_speedMin = 0.f;
	particle_type->m_speedMax = 200.f;
	particle_type->m_sizeRange = .5f;
	particle_type->m_lifeMin = 1.f;
	particle_type->m_lifeMax = 10.f;
	particle_type->m_directionRange = cherrysoda::Math::Pi2;
	particle_type->m_colorMode = cherrysoda::ParticleType::ColorModes::Fade;
	particle_type->m_rotationMode = cherrysoda::ParticleType::RotationModes::Random;
	particle_type->m_acceleration = cherrysoda::Math::Vec2(0.f, -100.f);
	particle_type->m_scaleOut = true;
	particle_type->m_source = sprite->Texture();

	auto particle_emitter = new cherrysoda::ParticleEmitter(particle_system, particle_type,
															cherrysoda::Math::Vec2(0.0f, 0.0f),
															cherrysoda::Math::Vec2(-50.0f, 50.0f), 500, 1.0f);

	auto particle_entity = new cherrysoda::Entity();
	particle_entity->Add(particle_emitter);
	particle_entity->AddTag(++g_iEntityID);


	// Add to scene the Particle Entity and System to get desired effect.
	scene->Add(particle_entity);
	scene->Add(particle_system);



	auto dude = new cherrysoda::Entity();
	dude->AddTag(++g_iEntityID);
	m_entityNameTagMap.emplace("Walter", g_iEntityID);

	scene->Add(dude);
	*/
}


void App::LoadContent()
{
	// Load only if Initialization went OK.
	if (!App::ShouldExit())
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
	if (!App::ShouldExit())
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
	if (!App::ShouldExit())
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
	if (!App::ShouldExit())
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

void StateMachine::Transit(const cherrysoda::String& name)
{
	if (cherrysoda::STL::Find(m_currentState->m_transitions, name) != m_currentState->m_transitions.end())
	{
		// Delete previous state.
		delete m_previousState;
		m_previousState = nullptr;

		m_previousState = m_currentState;
		m_currentState = m_states[name];


		// Call unloading and loading functions async and wait for them to end on end.
		//auto on_end_result = std::async(&State::OnEnd, m_previousState);
		//auto on_begin_result = std::async(&State::OnBegin, m_currentState);

		//m_previousState->OnEnd();
		//m_currentState->OnBegin();

		// Wait for loading to finish. Unloading can proceed in the background.
		//on_begin_result.wait();

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
void InitializationScene::SceneImpl::Begin()
{
	LOG_GAME_INFO("[InitializationScene] Begin");
}
void InitializationScene::SceneImpl::End()
{
	LOG_GAME_INFO("[InitializationScene] End");
}
void InitializationScene::SceneImpl::Update()
{
	static bool show_demo;
	ImGui::ShowDemoWindow(&show_demo);


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
void SplashSceenScene::SceneImpl::Update()
{
	if (!m_initializationComplete) return;

	// Base update.
	cherrysoda::Scene::Update();


	if (!m_application->Incoming().empty())
	{
		auto msg = m_application->Incoming().pop_front().msg;

		switch (msg.header.id)
		{
		case net::Message::NET_MSG_MAPDATA:
		{
			net::GameDesc desc;

			msg >> desc;

			printf("%s", desc.m_mapdata.c_str());
		}
		}
	}
}
void SplashSceenScene::SceneImpl::Begin()
{
	LOG_GAME_INFO("[SplashSceenScene] Begin");

	using namespace cherrysoda;
	auto factory = Factory::get();

	// Event System.
	auto event_system = factory->Begin(this)
		.Add(new EventSystem())
		.End();

	// Net Entity.
	auto chin = factory->Begin(this)
		.Add(new Unit("Chinperator", 1, 50, 200, 25, 25, 2, 1, 256.0f, 128.0f))
		.Add(new Observable(event_system->Tag()))
		.End();

	// Observer.
	auto observer = factory->Begin(this)
		.Add(new Observer("NetGameobjectUpdate", chin->Get< Observable >()->GetNetId()))
		.End();

	// Register Observer.
	event_system->Get< EventSystem >()->Add(observer->Get< Observer >(), "NetGameobjectUpdate");


	// TEST: REQUEST MAP DATA FROM SERVER.
	olc::net::message < net::Message > msg;
	msg.header.id = net::Message::NET_MSG_MAPDATA;
	m_application->Send(msg);


	/*
	// TESTING
	// Make Entity (EventSystem)
	auto event_system = new cherrysoda::Entity();
	event_system->AddTag(++g_iEntityID);
	auto event_system_id = g_iEntityID;
	event_system->Add(new cherrysoda::EventSystem());


	// Make Entity (Observable unit)
	auto e = new cherrysoda::Entity();
	e->AddTag(++g_iEntityID);
	auto unit = new cherrysoda::Unit();
	unit->SetUnitName("Chinperator");
	unit->SetHealth(20);
	unit->SetArmor(250);
	unit->SetDefense(50);
	unit->SetAttack(30);
	unit->SetTilePositionX(1);
	unit->SetTilePositionY(18);
	unit->SetPositionX(128.0f);
	unit->SetPositionY(128.0f * 18);
	unit->SetPlayerId(1);
	e->Add(unit);
	e->Add(new cherrysoda::Observable(event_system_id));

	// Make Entity (Observer)
	auto listener = new cherrysoda::Entity();
	listener->AddTag(++g_iEntityID);
	listener->Add(new cherrysoda::Observer("NetGameobjectUpdate", e->Get< cherrysoda::Observable >()->GetNetId()));

	// Add EventListener(Observer)
	event_system->Get< cherrysoda::EventSystem >()->Add(listener->Get< cherrysoda::Observer >(), "NetGameobjectUpdate");

	// Add Entity to Scene.
	Add(e);
	Add(event_system);
	Add(listener);
	// TESTING END
	*/


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


	/*
	// Create Entity for Text.
	auto text1 = new cherrysoda::Entity();
	auto text2 = new cherrysoda::Entity();
	auto text3 = new cherrysoda::Entity();
	auto text4 = new cherrysoda::Entity();
	text1->AddTag(++g_iEntityID);
	text2->AddTag(++g_iEntityID);
	text3->AddTag(++g_iEntityID);
	text4->AddTag(++g_iEntityID);


	// Create Text.
	Localization::SetLanguage("English");
	cherrysoda::String s = Localization::GetLocalizedString("Main Menu");			  // Eng.
	Localization::SetLanguage("Russian");
	cherrysoda::String ss = Localization::GetLocalizedString("Options");			  // Ru.
	cherrysoda::String sss = Localization::GetLocalizedString("Single Player");		  // Ru.
	Localization::SetLanguage("German");
	cherrysoda::String ssss = Localization::GetLocalizedString("Main Menu");		  // Germ.

	auto t = new cherrysoda::PixelText(m_application->m_font, s.c_str(), cherrysoda::Color::Red);
	t->Position(cherrysoda::Math::Vec2(-100.0f, -100.0f));
	t->Size(32.0f);

	auto tt = new cherrysoda::PixelText(m_application->m_font, ss.c_str(), cherrysoda::Color::Yellow);
	tt->Position(cherrysoda::Math::Vec2(10.0f, 1.0f));
	tt->Size(32.0f);

	auto ttt = new cherrysoda::PixelText(m_application->m_font, sss.c_str(), cherrysoda::Color::Blue);
	ttt->Position(cherrysoda::Math::Vec2(250.0f, 35.0f));
	ttt->Size(32.0f);

	auto tttt = new cherrysoda::PixelText(m_application->m_font, ssss.c_str(), cherrysoda::Color::Green);
	tttt->Position(cherrysoda::Math::Vec2(0.0f, 300.0f));
	tttt->Size(32.0f);



	text1->Add(t);
	text2->Add(tt);
	text3->Add(ttt);
	text4->Add(tttt);


	// Add Text to Scene.
	Add(text1);
	Add(text2);
	Add(text3);
	Add(text4);
	*/

	m_initializationComplete = true;
}
void SplashSceenScene::SceneImpl::End()
{
	LOG_GAME_INFO("[SplashSceenScene] End");
}

using GameApp = App;
CHERRYSODA_DEFAULT_MAIN