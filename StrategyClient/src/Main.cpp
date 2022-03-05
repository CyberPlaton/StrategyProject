#include "Main.h"

void NetCommMngr::UpdateNetGameobject(net::NetGameobject& object)
{
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
}
void App::Update()
{
	using namespace std;
	
	// ENGINE UPDATE
	base::Update();
	
	// STEAM UPDATE
	SteamAPI_RunCallbacks();

	
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
	// Initialize Localization (So we can provide Localized Error Messages)
	if (!InitializeLocalization())
	{
		App::Exit();
		return;
	}
	printf("Localization initialized\n");

	// Initialize the Platform Client.
	if (!InitializePlatformClient())
	{
		App::Exit();
		return;
	}
	printf("Platform Client initialized\n");

	// Initialize the GamerService.
	if (!InitializeGamerService())
	{
		App::Exit();
		return;
	}
	printf("GamerService initialized\n");

	if (!InitializeMasterConnection())
	{
		App::Exit();
		return;
	}
	printf("MasterServer initialized\n");


	if (!NetCommMngr::Initialize(this))
	{
		App::Exit();
		return;
	}
	printf("NetCommMngr initialized\n");

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
	TerminatePlatformClient();

	// DELETE GAMERSERVICE
	TerminateGamerService();

	// DELTET MASTERSERVER CONNECTION
	TerminateMasterConnection();

	// DELETE STEAM
	SteamAPI_Shutdown();

	// DELETE LOCALIZATION
	TerminateLocalization();
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
		printf("[App::InitializeMasterConnection] Starting Connection Attempt!\n");

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
		auto on_end_result = std::async(&State::OnEnd, m_previousState);
		auto on_begin_result = std::async(&State::OnBegin, m_currentState);

		//m_previousState->OnEnd();
		//m_currentState->OnBegin();

		// Wait for loading to finish. Unloading can proceed in the background.
		on_begin_result.wait();


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


// SCENE FUNCTION IMPLEMENTATIONS
void InitializationScene::SceneImpl::Begin()
{
	printf("[InitializationScene] Begin\n");
}
void InitializationScene::SceneImpl::Update()
{
	printf("[InitializationScene] Update\n");

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
				printf("[InitializationScene] User Data for validation requested!\n");
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
				printf("[InitializationScene] Received rejection!\n");
				m_application->Exit();
				break;


			// We were accepted and received our user data from Database.
			case net::NET_MSG_USER_DATA:
			{
				printf("[InitializationScene] Received User Data!\n");

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
		printf("[InitializationScene] Initialization Complete!\n");
		m_stateMachine->Transit("SplashScreen");
	}
}
void SplashSceenScene::SceneImpl::Update()
{
	if (!m_initializationComplete) return;
	printf("[SplashSceenScene] Update\n");

	static bool show_demo;
	ImGui::ShowDemoWindow(&show_demo);



	// TESTING CODE:
	// SEND A GAMEOBJECTS DATA TO SERVER AND OBSERVER ANY
	// DYNAMIC CHANGE IN CLIENT IN THE SERVER.
	int armor, defense, attack, health;
	armor = m_application->m_NetGameobject->m_unitArmor;
	defense = m_application->m_NetGameobject->m_unitDefense;
	attack = m_application->m_NetGameobject->m_unitAttack;
	health = m_application->m_NetGameobject->m_unitHealth;
	ImGui::Begin("NetGameobject Data");
	ImGui::SliderInt("Armor", &armor, 1, 1000);
	ImGui::SliderInt("Defense", &defense, 1, 1000);
	ImGui::SliderInt("Attack", &attack, 1, 1000);
	ImGui::SliderInt("Health", &health, 1, 1000);
	ImGui::End();
	m_application->m_NetGameobject->m_unitArmor = armor;
	m_application->m_NetGameobject->m_unitAttack = attack;
	m_application->m_NetGameobject->m_unitDefense = defense;
	m_application->m_NetGameobject->m_unitHealth = health;

	olc::net::message < net::Message > msg;
	msg.header.id = net::Message::NET_MSG_GAMEOBJECT_UPDATE;
	msg << *m_application->m_NetGameobject;


	m_application->Send(msg);


	ImGui::Begin("Test");
	ImGui::Text("Hello World");
	ImGui::End();

	std::vector< net::NetGameobject > mapdata;
	mapdata.resize(5);
	for (int i = 0; i < 5; i++)
	{
		mapdata[i].m_MaptileType = "Ground";
		mapdata[i].m_MaptileBiome = "Temperate";
		mapdata[i].m_netId = net::CreateNetworkUUID();
		mapdata[i].m_tilePositionX = i;
		mapdata[i].m_tilePositionY = 1;
		mapdata[i].m_positionX = i;
		mapdata[i].m_positionY = 2;
	}

	olc::net::message < net::Message > mapmsg;
	mapmsg.header.id = net::Message::NET_MSG_MAPDATA;
	mapmsg << mapdata;
	m_application->Send(mapmsg);


	// TESTING END
}
void SplashSceenScene::SceneImpl::Begin()
{
	printf("[SplashSceenScene] Begin\n");

	// Create a NetGameobjct
	m_application->m_NetGameobject = new net::NetGameobject();
	m_application->m_NetGameobject->m_name = "TestEntity";
	m_application->m_NetGameobject->m_unitName = "Chinnperator";
	m_application->m_NetGameobject->m_netId = net::CreateNetworkUUID();
	m_application->m_NetGameobject->m_playerId = m_application->m_localUserDesc->m_netId;
	m_application->m_NetGameobject->m_objectType = net::ENetGameobject::NET_GO_UNIT;
	m_application->m_NetGameobject->m_unitArmor = 200;
	m_application->m_NetGameobject->m_unitAttack = 20;
	m_application->m_NetGameobject->m_unitDefense = 50;
	m_application->m_NetGameobject->m_unitHealth = 50;


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


	m_initializationComplete = true;
}
void SplashSceenScene::SceneImpl::End()
{
	printf("[SplashSceenScene] End\n");
}

using GameApp = App;
CHERRYSODA_DEFAULT_MAIN